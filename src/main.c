#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "lib.h"

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

  // set metadata
  char *name = "Clock Overlay";
  char *version = "latest";
  char *identifier = "com.ownneufeldm.clockoverlay";
  if (!SDL_SetAppMetadata(name, version, identifier)) {
    return SDL_APP_FAILURE;
  }

  // configure style
  state->windowFlags = SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS;
  state->defaultGeometry = (SDL_Rect){.x = -5, .y = 5, .w = 0, .h = 0};
  state->defaultOpacity = 0.8f;
  state->foregroundColor = (SDL_Color){.r = 0, .g = 255, .b = 0};
  state->backgroundColor = (SDL_Color){.r = 0, .g = 0, .b = 0};
  state->timeFormat = "%H:%M";
  state->timeReference = "88:88";
  state->iconFile = "assets/icon.png";
  state->fontFile = "assets/font.ttf";
  state->fontSize = 16;

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
