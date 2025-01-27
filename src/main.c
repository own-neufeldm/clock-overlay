#define SDL_MAIN_USE_CALLBACKS
#define BUFFER_LENGTH 100

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
 * Application state for passing data throughout callbacks.
 *
 * \param tbc
 */
typedef struct {
  // style configured by developer
  SDL_WindowFlags windowFlags;
  SDL_Rect defaultGeometry;
  float defaultOpacity;
  SDL_Color foregroundColor;
  SDL_Color backgroundColor;
  const char *timeFormat;
  const char *timeReference;
  const char *fontFile;
  int fontSize;

  // resources managed by application
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
  SDL_Texture *texture;
  char text[BUFFER_LENGTH];
  bool changePosition;
  SDL_Rect requestedGeometry;
  SDL_Point relativeMousePosition;
  bool changeOpacity;
  float requestedOpacity;
} AppState;

/**
 * Loads the font used for rendering text.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool loadFont(AppState *state) {
  state->font = TTF_OpenFont(state->fontFile, state->fontSize);
  return state->font != NULL;
}

/**
 * Loads the main window of this application. Requires font to be loaded.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool loadWindow(AppState *state) {
  // normalize default size
  if (state->defaultGeometry.w == 0 || state->defaultGeometry.h == 0) {
    const char *text = state->timeReference;
    size_t length = 0;
    int w, h;
    if (!TTF_GetStringSize(state->font, text, length, &w, &h)) {
      return false;
    }
    if (state->defaultGeometry.w == 0) {
      state->defaultGeometry.w = w;
    }
    if (state->defaultGeometry.h == 0) {
      state->defaultGeometry.h = h;
    }
  }

  // create window, hidden
  const char *title = NULL;
  int w = state->defaultGeometry.w;
  int h = state->defaultGeometry.h;
  SDL_WindowFlags flags = state->windowFlags | SDL_WINDOW_HIDDEN;
  state->window = SDL_CreateWindow(title, w, h, flags);
  if (state->window == NULL) {
    return false;
  }

  // normalize default position
  if (state->defaultGeometry.x < 0 || state->defaultGeometry.y < 0) {
    SDL_DisplayID displayID = SDL_GetDisplayForWindow(state->window);
    const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(displayID);
    if (displayMode == NULL) {
      return false;
    }
    if (state->defaultGeometry.x < 0) {
      state->defaultGeometry.x += displayMode->w - state->defaultGeometry.w;
    }
    if (state->defaultGeometry.y < 0) {
      state->defaultGeometry.y += displayMode->h - state->defaultGeometry.h;
    }
  }

  // initialize dynamic variables with default values
  state->requestedGeometry = state->defaultGeometry;
  state->requestedOpacity = state->defaultOpacity;
  return true;
}

/**
 * Loads the default renderer. Requires window to be loaded.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool loadRenderer(AppState *state) {
  // create renderer
  const char *name = NULL;
  state->renderer = SDL_CreateRenderer(state->window, name);
  if (state->renderer == NULL) {
    return false;
  }

  // set color properties
  Uint8 r = state->backgroundColor.r;
  Uint8 g = state->backgroundColor.g;
  Uint8 b = state->backgroundColor.b;
  Uint8 a = state->backgroundColor.a;
  return SDL_SetRenderDrawColor(state->renderer, r, g, b, a);
}

/**
 * Update window position to requested value, but only if it differs.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool updatePosition(AppState *state) {
  int cx, cy;
  if (!SDL_GetWindowPosition(state->window, &cx, &cy)) {
    return false;
  }

  int rx = state->requestedGeometry.x;
  int ry = state->requestedGeometry.y;
  if (cx == rx && cy == ry) {
    return true;
  }

  return SDL_SetWindowPosition(state->window, rx, ry);
}

/**
 * Update window opacity to requested value, but only if it differs.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool updateOpacity(AppState *state) {
  float currentOpacity = SDL_GetWindowOpacity(state->window);
  if (currentOpacity == -1.0f) {
    return false;
  }

  if (currentOpacity == state->requestedOpacity) {
    return true;
  }

  return SDL_SetWindowOpacity(state->window, state->requestedOpacity);
}

/**
 * Render to the buffer, creating a new texture first if time has changed.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool renderTexture(AppState *state) {
  // format new time
  size_t length = BUFFER_LENGTH;
  char text[length];
  time_t now = time(NULL);
  strftime(text, length, state->timeFormat, localtime(&now));

  // ensure texture is up to date
  if (strcmp(text, state->text) != 0) {
    strcpy(state->text, text);

    // render text to surface
    SDL_Color fg = state->foregroundColor;
    SDL_Surface *surface = TTF_RenderText_Solid(state->font, text, length, fg);
    if (surface == NULL) {
      return false;
    }

    // convert surface to texture
    state->texture = SDL_CreateTextureFromSurface(state->renderer, surface);
    if (state->texture == NULL) {
      return true;
    }
    SDL_DestroySurface(surface);
  }

  // render entire texture to entire buffer
  SDL_FRect *srcrect = NULL;
  SDL_FRect *dstrect = NULL;
  return SDL_RenderTexture(state->renderer, state->texture, srcrect, dstrect);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  // allocate application state
  AppState *state = SDL_calloc(1, sizeof(AppState));
  if (state == NULL) {
    return SDL_APP_FAILURE;
  }
  *appstate = state;

  // initialize system
  if (!SDL_Init(SDL_INIT_VIDEO) || !TTF_Init()) {
    return SDL_APP_FAILURE;
  }

  // configure style
  state->windowFlags = SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS;
  state->defaultGeometry = (SDL_Rect){.x = -5, .y = 5, .w = 0, .h = 0};
  state->defaultOpacity = 0.8f;
  state->foregroundColor = (SDL_Color){.r = 0, .g = 255, .b = 0};
  state->backgroundColor = (SDL_Color){.r = 0, .g = 0, .b = 0};
  state->timeFormat = "%H:%M:%S";
  state->timeReference = "88:88:88";
  state->fontFile = "assets/Roboto/static/Roboto-Regular.ttf";  // TODO: change
  state->fontSize = 12;

  // load resources
  if (!loadFont(state) || !loadWindow(state) || !loadRenderer(state)) {
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = (AppState *)appstate;

  // reset buffer
  if (!SDL_RenderClear(state->renderer)) {
    return SDL_APP_FAILURE;
  }

  // make updates, if necessary, and render to buffer
  if (!updatePosition(state)) {
    return SDL_APP_FAILURE;
  }
  if (!updateOpacity(state)) {
    return SDL_APP_FAILURE;
  }
  if (!renderTexture(state)) {
    return SDL_APP_FAILURE;
  }

  // draw buffer and show window, if hidden
  if (!SDL_RenderPresent(state->renderer)) {
    return SDL_APP_FAILURE;
  }
  if (!SDL_ShowWindow(state->window)) {
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  AppState *state = (AppState *)appstate;

  switch (event->type) {
    // application process was terminated
    case SDL_EVENT_QUIT:
      return SDL_APP_SUCCESS;
      break;

    case SDL_EVENT_KEY_DOWN:
      switch (event->key.key) {
        // user wants to change the window opacity
        case SDLK_LCTRL:
        case SDLK_RCTRL:
          state->changeOpacity = true;
          break;
      }
      break;

    case SDL_EVENT_KEY_UP:
      switch (event->key.key) {
        // user finished changing the window opacity
        case SDLK_LCTRL:
        case SDLK_RCTRL:
          state->changeOpacity = false;
          break;
      }
      break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      switch (event->button.button) {
        // user wants to change the window position (drag)
        case SDL_BUTTON_LEFT:
          state->changePosition = true;
          state->relativeMousePosition.x = event->motion.x;
          state->relativeMousePosition.y = event->motion.y;
          break;
      }
      break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
      switch (event->button.button) {
        // user finished changing the window position (drag)
        case SDL_BUTTON_LEFT:
          state->changePosition = false;
          break;

        // user wants to quit the application
        case SDL_BUTTON_MIDDLE:
          return SDL_APP_SUCCESS;
          break;

        // user wants to reset a window property to its default value
        case SDL_BUTTON_RIGHT:
          if (state->changeOpacity) {
            state->requestedOpacity = state->defaultOpacity;
          } else {
            state->requestedGeometry = state->defaultGeometry;
          }
          break;
      }
      break;

    case SDL_EVENT_MOUSE_MOTION:
      // user wants to change the window position (drag)
      if (state->changePosition) {
        SDL_Point offset = {
            event->motion.x - state->relativeMousePosition.x,
            event->motion.y - state->relativeMousePosition.y,
        };
        state->requestedGeometry.x += offset.x;
        state->requestedGeometry.y += offset.y;
      }
      break;

    case SDL_EVENT_MOUSE_WHEEL:
      // user wants to change the window opacity
      // we enforce upper and lower boundaries for a smooth experience as well
      // as to avoid that the window becomes invisible and thus unreachable
      if (state->changeOpacity) {
        float factor = 0.1f;
        float newOpacity;
        if (event->wheel.y > 0) {
          newOpacity = state->requestedOpacity + factor;
          if (newOpacity > 1.0f) {
            newOpacity = 1.0f;
          }
        } else {
          newOpacity = state->requestedOpacity - factor;
          if (newOpacity < 0.1f) {
            newOpacity = 0.1f;
          }
        }
        state->requestedOpacity = newOpacity;
      }
      break;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  AppState *state = (AppState *)appstate;

  // if an SDL error occurred, log it
  if (result == SDL_APP_FAILURE) {
    fprintf(stderr, "[ERROR] %s\n", SDL_GetError());
  }

  // free TTF resources and quit system
  TTF_CloseFont(state->font);
  TTF_Quit();

  // free SDL resources and quit system
  SDL_DestroyTexture(state->texture);
  SDL_DestroyRenderer(state->renderer);
  SDL_DestroyWindow(state->window);
  SDL_free(state);
  SDL_Quit();
}
