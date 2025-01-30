#define BUFFER_LENGTH 100

#include "lib.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <time.h>

bool loadFont(AppState *state) {
  state->font = TTF_OpenFont(state->fontFile, state->fontSize);
  return state->font != NULL;
}

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

bool renderTexture(AppState *state) {
  // format new time
  char text[BUFFER_LENGTH];
  size_t length = BUFFER_LENGTH;
  time_t now = time(NULL);
  strftime(text, length, state->timeFormat, localtime(&now));

  // ensure texture is up to date
  if (strcmp(text, state->text) != 0) {
    strcpy(state->text, text);

    // render text to surface
    size_t length = 0;
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
