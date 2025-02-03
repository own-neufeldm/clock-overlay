#define BUFFER_LENGTH 100

#include "app.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <time.h>

bool loadFont(AppState *state) {
  state->font = TTF_OpenFont(state->fontFile, state->fontSize);
  return state->font != NULL;
}

bool normalizeDefaultSize(AppState *state) {
  // return if default size does not require normalization
  if (state->defaultGeometry.w != 0 && state->defaultGeometry.h != 0) {
    return true;
  }

  // determine text size
  const char *text = state->timeReference;
  size_t length = 0;
  int w, h;
  if (!TTF_GetStringSize(state->font, text, length, &w, &h)) {
    return false;
  }

  // update default size, if necessary
  if (state->defaultGeometry.w == 0) {
    state->defaultGeometry.w = w;
  }
  if (state->defaultGeometry.h == 0) {
    state->defaultGeometry.h = h;
  }

  return true;
}

bool normalizeDefaultPosition(AppState *state) {
  // return if default position does not require normalization
  if (state->defaultGeometry.x >= 0 && state->defaultGeometry.y >= 0) {
    return true;
  }

  // determine display size
  SDL_DisplayID displayID = SDL_GetDisplayForWindow(state->window);
  const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(displayID);
  if (displayMode == NULL) {
    return false;
  }

  // update default position, if necessary
  if (state->defaultGeometry.x < 0) {
    state->defaultGeometry.x += displayMode->w - state->defaultGeometry.w;
  }
  if (state->defaultGeometry.y < 0) {
    state->defaultGeometry.y += displayMode->h - state->defaultGeometry.h;
  }

  return true;
}

bool loadWindow(AppState *state) {
  // normalize default size, if necesary
  if (!normalizeDefaultSize(state)) {
    return false;
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

  // load window icon
  SDL_Surface *surface = IMG_Load(state->iconFile);
  if (surface == NULL) {
    return false;
  }
  bool ok = SDL_SetWindowIcon(state->window, surface);
  SDL_DestroySurface(surface);
  if (!ok) {
    return false;
  }

  // normalize default position, if necessary
  if (!normalizeDefaultPosition(state)) {
    return false;
  }

  // set requested values equal to default values -> no update required
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
  // determine window position
  int cx, cy;
  if (!SDL_GetWindowPosition(state->window, &cx, &cy)) {
    return false;
  }

  // return if position does not need an update
  int rx = state->requestedGeometry.x;
  int ry = state->requestedGeometry.y;
  if (cx == rx && cy == ry) {
    return true;
  }

  // udpate position
  return SDL_SetWindowPosition(state->window, rx, ry);
}

bool updateOpacity(AppState *state) {
  // determine window opacity
  float currentOpacity = SDL_GetWindowOpacity(state->window);
  if (currentOpacity == -1.0f) {
    return false;
  }

  // return if opacity does not need an update
  if (currentOpacity == state->requestedOpacity) {
    return true;
  }

  // update opacity
  return SDL_SetWindowOpacity(state->window, state->requestedOpacity);
}

bool updateTexture(AppState *state) {
  // format new time
  char text[BUFFER_LENGTH];
  size_t length = BUFFER_LENGTH;
  time_t now = time(NULL);
  strftime(text, length, state->timeFormat, localtime(&now));

  // return if texture does not need an update
  if (strcmp(text, state->text) == 0) {
    return true;
  }
  strcpy(state->text, text);

  // render text to surface
  size_t length = 0;
  SDL_Color fg = state->foregroundColor;
  SDL_Surface *surface = TTF_RenderText_Blended(state->font, text, length, fg);
  if (surface == NULL) {
    SDL_DestroySurface(surface);
    return false;
  }

  // convert surface to texture
  SDL_DestroyTexture(state->texture);
  state->texture = SDL_CreateTextureFromSurface(state->renderer, surface);
  SDL_DestroySurface(surface);
  if (state->texture == NULL) {
    return false;
  }

  return true;
}
