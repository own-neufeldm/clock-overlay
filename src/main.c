#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;

  bool changingWindowPosition;
  SDL_Point startChangeMousePosition;
  SDL_Point originWindowPosition;
  SDL_Point currentWindowPosition;

  bool changingWindowSize;
  SDL_Point originWindowSize;
  SDL_Point currentWindowSize;

  bool changingWindowOpacity;
  float originWindowOpacity;
  float currentWindowOpacity;

  unsigned int currentTime;
  unsigned int lastTime;
  SDL_Surface *textSurface;
  SDL_Texture *textTexture;
} AppState;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  // create application state
  AppState *state = SDL_calloc(1, sizeof(AppState));
  if (state == NULL) {
    return SDL_APP_FAILURE;
  }
  *appstate = state;

  // initialize SDL system
  if (!SDL_Init(SDL_INIT_VIDEO) || !TTF_Init()) {
    return SDL_APP_FAILURE;
  }

  // create window and renderer
  char *title = NULL;
  int width = 0;
  int height = 0;
  SDL_WindowFlags flags = SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS;
  SDL_CreateWindowAndRenderer(title, width, height, flags, &(state->window),
                              &(state->renderer));
  if (state->window == NULL || state->renderer == NULL) {
    return SDL_APP_FAILURE;
  }
  if (SDL_GetWindowSurface(state->window) == NULL) {
    return SDL_APP_FAILURE;
  }

  // open font
  char *fontFile = "assets/Roboto/static/Roboto-Regular.ttf";
  int fontSize = 72;
  state->font = TTF_OpenFont(fontFile, fontSize);
  if (state->font == NULL) {
    return SDL_APP_FAILURE;
  }

  // get display size
  SDL_DisplayID displayId = SDL_GetDisplayForWindow(state->window);
  const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(displayId);
  if (displayMode == NULL) {
    return SDL_APP_FAILURE;
  }
  SDL_Point displaySize = {displayMode->w, displayMode->h};

  // set window properties
  state->originWindowPosition = (SDL_Point){100, 100};
  state->currentWindowPosition = state->originWindowPosition;
  state->originWindowSize = (SDL_Point){100, 100};
  state->currentWindowSize = state->originWindowSize;
  state->originWindowOpacity = 0.5f;
  state->currentWindowOpacity = state->originWindowOpacity;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  AppState *state = (AppState *)appstate;

  switch (event->type) {
    case SDL_EVENT_QUIT:
      return SDL_APP_SUCCESS;
      break;

    case SDL_EVENT_KEY_DOWN:
      switch (event->key.key) {
        case SDLK_LCTRL:
        case SDLK_RCTRL:
          state->changingWindowSize = true;
          break;

        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
          state->changingWindowOpacity = true;
          break;
      }
      break;

    case SDL_EVENT_KEY_UP:
      switch (event->key.key) {
        case SDLK_LCTRL:
        case SDLK_RCTRL:
          state->changingWindowSize = false;
          break;

        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
          state->changingWindowOpacity = false;
          break;
      }
      break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      switch (event->button.button) {
        case SDL_BUTTON_LEFT:
          state->changingWindowPosition = true;
          SDL_Point mousePosition = {event->motion.x, event->motion.y};
          state->startChangeMousePosition = mousePosition;
          break;
      }
      break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
      switch (event->button.button) {
        case SDL_BUTTON_LEFT:
          state->changingWindowPosition = false;
          break;

        case SDL_BUTTON_MIDDLE:
          return SDL_APP_SUCCESS;
          break;

        case SDL_BUTTON_RIGHT:
          if (state->changingWindowSize) {
            state->currentWindowSize = state->originWindowSize;
          } else if (state->changingWindowOpacity) {
            state->currentWindowOpacity = state->originWindowOpacity;
          } else {
            state->currentWindowPosition = state->originWindowPosition;
          }
          break;
      }
      break;

    case SDL_EVENT_MOUSE_MOTION:
      if (state->changingWindowPosition) {
        SDL_Point offset = {
            event->motion.x - state->startChangeMousePosition.x,
            event->motion.y - state->startChangeMousePosition.y,
        };
        state->currentWindowPosition.x += offset.x;
        state->currentWindowPosition.y += offset.y;
      }
      break;

    case SDL_EVENT_MOUSE_WHEEL:
      if (state->changingWindowSize) {
        float factor = 1.1f;
        if (event->wheel.y > 0) {
          state->currentWindowSize.x *= factor;
          state->currentWindowSize.y *= factor;
        } else {
          state->currentWindowSize.x /= factor;
          state->currentWindowSize.y /= factor;
        }

      } else if (state->changingWindowOpacity) {
        float factor = 0.1f;
        if (event->wheel.y > 0) {
          float newOpacity = state->currentWindowOpacity + factor;
          if (newOpacity > 1.0f) {
            newOpacity = 1.0f;
          }
          state->currentWindowOpacity = newOpacity;
        } else {
          float newOpacity = state->currentWindowOpacity - factor;
          if (newOpacity < 0.1f) {
            newOpacity = 0.1f;
          }
          state->currentWindowOpacity = newOpacity;
        }
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
  int x = state->currentWindowPosition.x;
  int y = state->currentWindowPosition.y;
  if (!SDL_SetWindowPosition(state->window, x, y)) {
    return SDL_APP_FAILURE;
  }

  // update window size
  int w = state->currentWindowSize.x;
  int h = state->currentWindowSize.y;
  if (!SDL_SetWindowSize(state->window, w, h)) {
    return SDL_APP_FAILURE;
  }

  // update window opacity
  float opacity = state->currentWindowOpacity;
  if (!SDL_SetWindowOpacity(state->window, opacity)) {
    return SDL_APP_FAILURE;
  }

  // update texture if necessary
  state->currentTime = SDL_GetTicks();
  if (state->lastTime == 0 || state->currentTime - state->lastTime > 1000) {
    state->lastTime = state->currentTime;

    // format new time
    time_t now = time(NULL);
    size_t length = 9;
    char text[length];
    strftime(text, length, "%H:%M:%S", localtime(&now));

    // render text to surface
    SDL_Color color = {.r = 255, .g = 255, .b = 255, .a = 255};
    state->textSurface = TTF_RenderText_Solid(state->font, text, length, color);
    if (state->textSurface == NULL) {
      return SDL_APP_FAILURE;
    }

    // convert surface to texture
    state->textTexture =
        SDL_CreateTextureFromSurface(state->renderer, state->textSurface);
    if (state->textTexture == NULL) {
      return SDL_APP_FAILURE;
    }
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

  if (result == SDL_APP_FAILURE) {
    fprintf(stderr, "[ERROR] %s\n", SDL_GetError());
  }

  SDL_free(state);
  SDL_Quit();
  TTF_Quit();
}
