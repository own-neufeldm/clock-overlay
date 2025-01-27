#define SDL_MAIN_USE_CALLBACKS

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
  TTF_Font *font;
  SDL_Window *window;
  SDL_Renderer *renderer;

  bool changePosition;
  SDL_Point relativeMousePosition;
  SDL_Rect defaultGeometry;
  SDL_Rect requestedGeometry;

  bool changeOpacity;
  float defaultOpacity;
  float requestedOpacity;

  Uint64 lastUpdate;
  SDL_Color foregroundColor;
  SDL_Color backgroundColor;
  const char *format;
  size_t length;
  SDL_Texture *texture;
} AppState;

/**
 * Loads the font used for rendering text.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool loadFont(AppState *state) {
  // TODO: create own font or use something with MIT license
  const char *file = "assets/Roboto/static/Roboto-Regular.ttf";
  int ptsize = 12;

  state->font = TTF_OpenFont(file, ptsize);
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
  // determine minimal window size
  const char *text = "88:88:88";
  SDL_Point size = {};
  if (!TTF_GetStringSize(state->font, text, state->length, &size.x, &size.y)) {
    return false;
  }

  // create window, hidden
  const char *title = NULL;
  SDL_WindowFlags flags = (
    SDL_WINDOW_ALWAYS_ON_TOP |
    SDL_WINDOW_BORDERLESS |
    SDL_WINDOW_HIDDEN
    );
  state->window = SDL_CreateWindow(title, size.x, size.y, flags);
  if (state->window == NULL) {
    return false;
  }

  // determine position in upper-right corner of the screen
  SDL_DisplayID displayID = SDL_GetDisplayForWindow(state->window);
  const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(displayID);
  if (displayMode == NULL) {
    return false;
  }
  SDL_Point position = { displayMode->w - size.x - 5, 5 };


  // update window properties
  state->defaultGeometry = (SDL_Rect){ size.x, size.y, position.x, position.y };
  state->requestedGeometry = state->defaultGeometry;
  if (!SDL_SetWindowPosition(state->window, position.x, position.y)) {
    return false;
  }
  state->requestedOpacity = state->defaultOpacity;
  if (!SDL_SetWindowOpacity(state->window, state->defaultOpacity)) {
    return false;
  }

  // show window
  return SDL_ShowWindow(state->window);
}

/**
 * Loads the default renderer. Requires the main window to be loaded.
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

  // update renderer properties
  return SDL_SetRenderDrawColor(
    state->renderer,
    state->backgroundColor.r,
    state->backgroundColor.g,
    state->backgroundColor.b,
    state->backgroundColor.a
  );
}

/**
 * Apply the requested window position, but only if a change is necessary.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool updatePosition(AppState *state) {
  if (!state->changePosition) {
    return true;
  }

  return SDL_SetWindowPosition(
    state->window,
    state->requestedGeometry.x,
    state->requestedGeometry.y
  );
}

/**
 * Apply the requested window opacity, but only if a change is necessary.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool updateOpacity(AppState *state) {
  if (!state->changeOpacity) {
    return true;
  }

  printf("Updating opacity to %f\n", state->requestedOpacity);
  return SDL_SetWindowOpacity(
    state->window,
    state->requestedOpacity
  );
}

/**
 * Create a new texture containing the time, but only if a change is necessary.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool updateTexture(AppState *state) {
  // update text every 1 second(s)
  Uint64 currentUpdate = SDL_GetTicks();
  if (state->lastUpdate != 0 && currentUpdate < state->lastUpdate + 1000) {
    return true;
  }
  state->lastUpdate = currentUpdate;

  // format new time
  time_t now = time(NULL);
  char text[state->length];
  strftime(text, state->length, state->format, localtime(&now));

  // render text to surface
  SDL_Surface *surface = TTF_RenderText_Solid(
    state->font,
    text,
    state->length,
    state->foregroundColor
  );
  if (surface == NULL) {
    return false;
  }

  // convert surface to texture
  state->texture = SDL_CreateTextureFromSurface(state->renderer, surface);
  if (state->texture == NULL) {
    return true;
  }

  // free temporary resources
  SDL_DestroySurface(surface);
  return true;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  // allocate application state
  AppState *state = SDL_calloc(1, sizeof(AppState));
  if (state == NULL) {
    return SDL_APP_FAILURE;
  }
  *appstate = state;

  // set application properties
  state->defaultOpacity = 0.8f;
  state->foregroundColor = (SDL_Color){ .r = 0, .g = 255, .b = 0, .a = 255 };
  state->backgroundColor = (SDL_Color){ .r = 0, .g = 0, .b = 0, .a = 255 };
  state->format = "%H:%M:%S";
  state->length = 9; // HH:MM:SS\0

  // initialize system
  if (!SDL_Init(SDL_INIT_VIDEO) || !TTF_Init()) {
    return SDL_APP_FAILURE;
  }

  // load resources
  if (!loadFont(state) || !loadWindow(state) || !loadRenderer(state)) {
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

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = (AppState *)appstate;

  // reset buffer
  if (!SDL_RenderClear(state->renderer)) {
    return SDL_APP_FAILURE;
  }

  // update window properties, if necessary
  if (!updatePosition(state) || !updateOpacity(state)) {
    return SDL_APP_FAILURE;
  }

  // update texture, if necessary
  if (!updateTexture(state)) {
    return SDL_APP_FAILURE;
  }

  // write entire texture to entire buffer 
  SDL_FRect *srcrect = NULL;
  SDL_FRect *dstrect = NULL;
  if (!SDL_RenderTexture(state->renderer, state->texture, srcrect, dstrect)) {
    return SDL_APP_FAILURE;
  }

  // draw buffer
  if (!SDL_RenderPresent(state->renderer)) {
    return SDL_APP_FAILURE;
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
