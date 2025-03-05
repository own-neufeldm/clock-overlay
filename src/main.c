#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "app.h"

static const char *NAME = "Clock Overlay";
static const char *VERSION = "v2.3.2";
static const char *IDENTIFIER = "com.ownneufeldm.clockoverlay";

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  // parse command-line arguments
  bool debug = false;
  if (argc == 2 && strcmp(argv[1], "--debug") == 0) {
    debug = true;
  }

  // allocate application state
  AppState *state = SDL_calloc(1, sizeof(AppState));
  if (state == NULL) {
    SDL_Log("[ERROR] Unable to allocate app state: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  *appstate = state;

  // initialize system
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("[ERROR] Unable to initialize SDL: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  if (!TTF_Init()) {
    SDL_Log("[ERROR] Unable to initialize TTF: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // set metadata
  if (!SDL_SetAppMetadata(NAME, VERSION, IDENTIFIER)) {
    SDL_Log("[ERROR] Unable to set metadata: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // configure style
  state->windowFlags = SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS;
  state->defaultGeometry = (SDL_Rect){.x = -5, .y = 5, .w = 0, .h = 0};
  state->foregroundColor = (SDL_Color){.r = 0, .g = 255, .b = 0};
  state->backgroundColor = (SDL_Color){.r = 0, .g = 0, .b = 0};
  state->timeFormat = debug ? "%H:%M:%S" : "%H:%M";
  state->timeReference = debug ? "88:88:88" : "88:88";
  state->iconFile = "assets/icon.png";
  state->fontFile = "assets/font.ttf";
  state->fontSize = 16;

  // load resources
  if (!loadFont(state)) {
    SDL_Log("[ERROR] Unable to load font: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  if (!loadWindow(state)) {
    SDL_Log("[ERROR] Unable to load window: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  if (!loadRenderer(state)) {
    SDL_Log("[ERROR] Unable to load renderer: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = (AppState *)appstate;

  // reset buffer
  if (!SDL_RenderClear(state->renderer)) {
    SDL_Log("[ERROR] Unable to clear renderer: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // make updates, if necessary
  if (!updatePosition(state)) {
    SDL_Log("[ERROR] Unable to update position: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  if (!updateTexture(state)) {
    SDL_Log("[ERROR] Unable to update texture: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // render entire texture to entire buffer
  SDL_FRect *srcrect = NULL;
  SDL_FRect *dstrect = NULL;
  if (!SDL_RenderTexture(state->renderer, state->texture, srcrect, dstrect)) {
    SDL_Log("[ERROR] Unable to render texture: %s\n", SDL_GetError());
  }

  // present buffer
  if (!SDL_RenderPresent(state->renderer)) {
    SDL_Log("[ERROR] Unable to present renderer: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // show window, if hidden
  if (!SDL_ShowWindow(state->window)) {
    SDL_Log("[ERROR] Unable to show window: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  AppState *state = (AppState *)appstate;

  switch (event->type) {
    case SDL_EVENT_QUIT:
      return SDL_APP_SUCCESS;
      break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      switch (event->button.button) {
        // save relative mouse position before user starts dragging the window
        case SDL_BUTTON_LEFT:
          state->changePosition = true;
          state->relativeMousePosition.x = event->motion.x;
          state->relativeMousePosition.y = event->motion.y;
          break;
      }
      break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
      switch (event->button.button) {
        case SDL_BUTTON_LEFT:
          state->changePosition = false;
          break;

        case SDL_BUTTON_MIDDLE:
          return SDL_APP_SUCCESS;
          break;

        case SDL_BUTTON_RIGHT:
          state->requestedGeometry = state->defaultGeometry;
          break;
      }
      break;

    case SDL_EVENT_MOUSE_MOTION:
      // request a new position, if necessary
      if (state->changePosition) {
        SDL_Point offset = {
            event->motion.x - state->relativeMousePosition.x,
            event->motion.y - state->relativeMousePosition.y,
        };
        state->requestedGeometry.x += offset.x;
        state->requestedGeometry.y += offset.y;
      }
      break;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  AppState *state = (AppState *)appstate;

  // free TTF resources and quit system
  if (state->font != NULL) {
    TTF_CloseFont(state->font);
  }
  TTF_Quit();

  // free SDL resources and quit system
  SDL_DestroyTexture(state->texture);
  SDL_DestroyRenderer(state->renderer);
  SDL_DestroyWindow(state->window);
  SDL_free(state);
  SDL_Quit();
}
