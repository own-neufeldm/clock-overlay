#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// contains application data to avoid global variables
typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;

  bool changingWindowPosition;
  SDL_Point startChangeMousePosition;
  SDL_Point originWindowPosition;
  SDL_Point currentWindowPosition;

  // TODO: either remove or fix scaling, somehow aspect ratio is lost
  bool changingWindowSize;
  SDL_Point originWindowSize;
  SDL_Point currentWindowSize;

  bool changingWindowOpacity;
  float originWindowOpacity;
  float currentWindowOpacity;

  unsigned int currentTime;
  unsigned int lastTime;
  SDL_Color textColor;
  const char *textFormat;
  size_t textLength;
  SDL_Texture *textTexture;
} AppState;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  // create application state to share data throughout SDL callbacks
  AppState *state = SDL_calloc(1, sizeof(AppState));
  if (state == NULL) {
    return SDL_APP_FAILURE;
  }
  *appstate = state;

  // initialize SDL system, we only need video
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  // initialize TTF system
  if (!TTF_Init()) {
    return SDL_APP_FAILURE;
  }

  // create main window
  const char *title = NULL;
  int width = 0;
  int height = 0;
  SDL_WindowFlags flags = SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS;
  state->window = SDL_CreateWindow(title, width, height, flags);
  if (state->window == NULL) {
    return SDL_APP_FAILURE;
  }

  // create default renderer
  const char *name = NULL;
  state->renderer = SDL_CreateRenderer(state->window, name);
  if (state->renderer == NULL) {
    return SDL_APP_FAILURE;
  }

  // set render draw color, e.g. when resetting the buffer
  SDL_Color color = {.r = 0, .g = 0, .b = 0, .a = 255};
  if (!SDL_SetRenderDrawColor(state->renderer, color.r, color.g, color.b,
                              color.a)) {
    return SDL_APP_FAILURE;
  }

  // TODO: create own font or use something with MIT license
  // open font
  const char *file = "assets/Roboto/static/Roboto-Regular.ttf";
  int ptsize = 12;
  state->font = TTF_OpenFont(file, ptsize);
  if (state->font == NULL) {
    return SDL_APP_FAILURE;
  }

  // configure text formatting options
  state->textColor = (SDL_Color){.r = 0, .g = 255, .b = 0, .a = 255};
  state->textFormat = "%H:%M:%S";
  state->textLength = 9;  // HH:MM:SS\0

  // calculate the minimal window size for our text
  SDL_Point size = {};
  const char *string = "88:88:88";
  if (!TTF_GetStringSize(state->font, string, state->textLength, &(size.x),
                         &(size.y))) {
    return SDL_APP_FAILURE;
  }

  // calculate the window position in the upper-right corner of the display
  SDL_DisplayID displayId = SDL_GetDisplayForWindow(state->window);
  const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(displayId);
  if (displayMode == NULL) {
    return SDL_APP_FAILURE;
  }
  SDL_Point position = {displayMode->w - size.x - 10, 10};

  // set window properties
  state->originWindowPosition = position;
  state->currentWindowPosition = state->originWindowPosition;
  state->originWindowSize = size;
  state->currentWindowSize = state->originWindowSize;
  state->originWindowOpacity = 0.8f;
  state->currentWindowOpacity = state->originWindowOpacity;

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
        // user wants to change the window size
        case SDLK_LCTRL:
        case SDLK_RCTRL:
          state->changingWindowSize = true;
          break;

        // user wants to change the window opacity
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
          state->changingWindowOpacity = true;
          break;
      }
      break;

    case SDL_EVENT_KEY_UP:
      switch (event->key.key) {
        // user finished changing the window size
        case SDLK_LCTRL:
        case SDLK_RCTRL:
          state->changingWindowSize = false;
          break;

        // user finished changing the window opacity
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
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
      // user wants to change the window position (drag)
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
      // user wants to change the window size
      if (state->changingWindowSize) {
        float factor = 1.1f;
        if (event->wheel.y > 0) {
          state->currentWindowSize.x *= factor;
          state->currentWindowSize.y *= factor;
        } else {
          state->currentWindowSize.x /= factor;
          state->currentWindowSize.y /= factor;
        }

        // user wants to change the window opacity
        // we enforce upper and lower boundaries for a smooth experience as well
        // as to avoid that the window becomes invisible and thus unreachable
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

  // update text every 1 second(s)
  state->currentTime = SDL_GetTicks();
  if (state->lastTime == 0 || state->currentTime - state->lastTime > 1000) {
    state->lastTime = state->currentTime;

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
