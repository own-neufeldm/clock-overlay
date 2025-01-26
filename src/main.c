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
 * \param window main window.
 * \param renderer default renderer for the main window.
 * \param font font used for rendering text.
 * \param changingWindowPosition is the window being dragged by the user?
 * \param startChangeMousePosition mouse position before user started dragging.
 * \param originWindowGeometry default size and position of the window.
 * \param currentWindowGeometry current size and position of the window.
 * \param changingWindowOpacity is the user changing the windows opacity?
 * \param originWindowOpacity default opacity of the window.
 * \param currentWindowOpacity current opacity of the window.
 * \param lastUpdate milliseconds since the text was last updated.
 * \param textColor foreground color to use when rendering text.
 * \param textFormat format to apply to time when constructing text.
 * \param textLength buffer length to use when constructing text.
 * \param textTexture texture of text to render each frame.
 */
typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;

  bool changingWindowPosition;
  SDL_Point startChangeMousePosition;
  SDL_Rect originWindowGeometry;
  SDL_Rect currentWindowGeometry;

  bool changingWindowOpacity;
  float originWindowOpacity;
  float currentWindowOpacity;

  Uint64 lastUpdate;
  SDL_Color textColor;
  const char *textFormat;
  size_t textLength;
  SDL_Texture *textTexture;
} AppState;

/**
 * Creates the main window.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool createWindow(AppState *state) {
  const char *title = NULL;
  int w = 0;
  int h = 0;
  SDL_WindowFlags flags = SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS;

  state->window = SDL_CreateWindow(title, w, h, flags);
  return state->window != NULL;
}

/**
 * Creates the default renderer for the main window.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool createRenderer(AppState *state) {
  SDL_Window *window = state->window;
  const char *name = NULL;

  state->renderer = SDL_CreateRenderer(window, name);
  return state->renderer != NULL;
}

/**
 * Opens the font used for rendering text.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool openFont(AppState *state) {
  // TODO: create own font or use something with MIT license
  const char *file = "assets/Roboto/static/Roboto-Regular.ttf";
  int ptsize = 12;

  state->font = TTF_OpenFont(file, ptsize);
  return state->font != NULL;
}

/**
 * Sets text properties such as foreground color, format and length.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool setTextProperties(AppState *state) {
  state->textColor = (SDL_Color){.r = 0, .g = 255, .b = 0, .a = 255};
  state->textFormat = "%H:%M:%S";
  state->textLength = 9;  // HH:MM:SS\0
  return true;
}

/**
 * Sets window properties such as size, position and opacity.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool setWindowProperties(AppState *state) {
  TTF_Font *font = state->font;
  const char *text = "88:88:88";
  size_t length = state->textLength;
  int *w = &(state->originWindowGeometry.w);
  int *h = &(state->originWindowGeometry.h);
  if (!TTF_GetStringSize(font, text, length, w, h)) {
    return false;
  }

  SDL_Window *window = state->window;
  SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);
  const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(displayID);
  if (displayMode == NULL) {
    return false;
  }

  state->originWindowGeometry.x = displayMode->w - *w - 10;
  state->originWindowGeometry.y = 10;
  state->currentWindowGeometry = state->originWindowGeometry;
  state->originWindowOpacity = 0.8f;
  state->currentWindowOpacity = state->originWindowOpacity;
  return true;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  AppState *state = SDL_calloc(1, sizeof(AppState));
  if (state == NULL) {
    return SDL_APP_FAILURE;
  }
  *appstate = state;

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  if (!TTF_Init()) {
    return SDL_APP_FAILURE;
  }

  if (!createWindow(state)) {
    return SDL_APP_FAILURE;
  }

  if (!createRenderer(state)) {
    return SDL_APP_FAILURE;
  }

  if (!openFont(state)) {
    return SDL_APP_FAILURE;
  }

  if (!setTextProperties(state)) {
    return SDL_APP_FAILURE;
  }

  if (!setWindowProperties(state)) {
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
          state->changingWindowOpacity = true;
          break;
      }
      break;

    case SDL_EVENT_KEY_UP:
      switch (event->key.key) {
        // user finished changing the window opacity
        case SDLK_LCTRL:
        case SDLK_RCTRL:
          state->changingWindowOpacity = false;
          break;
      }
      break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      switch (event->button.button) {
        // user wants to change the window position (drag)
        case SDL_BUTTON_LEFT:
          state->changingWindowPosition = true;
          SDL_Point mousePosition = {event->motion.x, event->motion.y};
          state->startChangeMousePosition = mousePosition;
          break;
      }
      break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
      switch (event->button.button) {
        // user finished changing the window position (drag)
        case SDL_BUTTON_LEFT:
          state->changingWindowPosition = false;
          break;

        // user wants to quit the application
        case SDL_BUTTON_MIDDLE:
          return SDL_APP_SUCCESS;
          break;

        // user wants to reset a window property to its default value
        case SDL_BUTTON_RIGHT:
          if (state->changingWindowOpacity) {
            state->currentWindowOpacity = state->originWindowOpacity;
          } else {
            state->currentWindowGeometry = state->originWindowGeometry;
          }
          break;
      }
      break;

    case SDL_EVENT_MOUSE_MOTION:
      // user wants to change the window position (drag)
      if (state->changingWindowPosition) {
        SDL_Point offset = {
            event->motion.x - state->startChangeMousePosition.x,
            event->motion.y - state->startChangeMousePosition.y,
        };
        state->currentWindowGeometry.x += offset.x;
        state->currentWindowGeometry.y += offset.y;
      }
      break;

    case SDL_EVENT_MOUSE_WHEEL:
      // user wants to change the window opacity
      // we enforce upper and lower boundaries for a smooth experience as well
      // as to avoid that the window becomes invisible and thus unreachable
      if (state->changingWindowOpacity) {
        float factor = 0.1f;
        float newOpacity;
        if (event->wheel.y > 0) {
          newOpacity = state->currentWindowOpacity + factor;
          if (newOpacity > 1.0f) {
            newOpacity = 1.0f;
          }
        } else {
          newOpacity = state->currentWindowOpacity - factor;
          if (newOpacity < 0.1f) {
            newOpacity = 0.1f;
          }
        }
        state->currentWindowOpacity = newOpacity;
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

  // update window position
  int x = state->currentWindowGeometry.x;
  int y = state->currentWindowGeometry.y;
  if (!SDL_SetWindowPosition(state->window, x, y)) {
    return SDL_APP_FAILURE;
  }

  // update window size
  int w = state->currentWindowGeometry.w;
  int h = state->currentWindowGeometry.h;
  if (!SDL_SetWindowSize(state->window, w, h)) {
    return SDL_APP_FAILURE;
  }

  // update window opacity
  float opacity = state->currentWindowOpacity;
  if (!SDL_SetWindowOpacity(state->window, opacity)) {
    return SDL_APP_FAILURE;
  }

  // update text every 1 second(s)
  Uint64 currentUpdate = SDL_GetTicks();
  if (state->lastUpdate == 0 || currentUpdate - state->lastUpdate > 1000) {
    state->lastUpdate = currentUpdate;

    // format new time
    time_t now = time(NULL);
    char text[state->textLength];
    strftime(text, state->textLength, state->textFormat, localtime(&now));

    // render text to surface
    SDL_Surface *textSurface = TTF_RenderText_Solid(
        state->font, text, state->textLength, state->textColor);
    if (textSurface == NULL) {
      return SDL_APP_FAILURE;
    }

    // convert surface to texture
    state->textTexture =
        SDL_CreateTextureFromSurface(state->renderer, textSurface);
    if (state->textTexture == NULL) {
      return SDL_APP_FAILURE;
    }

    // free temporary resources
    SDL_DestroySurface(textSurface);
  }

  // write texture to buffer
  SDL_FRect *source = NULL;
  SDL_FRect *destination = NULL;
  if (!SDL_RenderTexture(state->renderer, state->textTexture, source,
                         destination)) {
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

  // free memory and quit TTF system
  TTF_CloseFont(state->font);
  TTF_Quit();

  // free memory and quit SDL system
  SDL_DestroyTexture(state->textTexture);
  SDL_DestroyRenderer(state->renderer);
  SDL_DestroyWindow(state->window);
  SDL_free(state);
  SDL_Quit();
}
