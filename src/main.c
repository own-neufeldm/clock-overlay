#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  SDL_WindowFlags flags =
      SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIDDEN;
  int width = 400, height = 300;
  float opacity = 0.5f;

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "[ERROR] Could not initialize video: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(NULL, width, height, flags);
  if (window == NULL) {
    fprintf(stderr, "[ERROR] Could not create window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Surface *surface = SDL_GetWindowSurface(window);
  if (surface == NULL) {
    fprintf(stderr, "[ERROR] Could not retrieve surface: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_DisplayID displayId = SDL_GetDisplayForWindow(window);
  if (displayId == 0) {
    fprintf(stderr, "[ERROR] Could not retrieve display: %s\n", SDL_GetError());
    SDL_DestroySurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(displayId);
  if (displayMode == NULL) {
    fprintf(stderr, "[ERROR] Could not read display: %s\n", SDL_GetError());
    SDL_DestroySurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  if (!SDL_SetWindowOpacity(window, opacity)) {
    fprintf(stderr, "[ERROR] Could not set opacity: %s\n", SDL_GetError());
    SDL_DestroySurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_Point defaultPosition = {.x = displayMode->w - width - 10, .y = 10};
  SDL_Point currentPosition = defaultPosition;
  SDL_Point beginDragPosition;
  bool leftMouseButtonDown = false;
  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          leftMouseButtonDown = true;
          beginDragPosition.x = event.motion.x;
          beginDragPosition.y = event.motion.y;
        }
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          leftMouseButtonDown = false;
        }
        if (event.button.button == SDL_BUTTON_MIDDLE) {
          running = false;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          currentPosition = defaultPosition;
        }
      } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
        if (leftMouseButtonDown) {
          SDL_Point offset = {.x = event.motion.x - beginDragPosition.x,
                              .y = event.motion.y - beginDragPosition.y};
          currentPosition.x = currentPosition.x + offset.x;
          currentPosition.y = currentPosition.y + offset.y;
        }
      }
    }

    if (!SDL_SetWindowPosition(window, currentPosition.x, currentPosition.y)) {
      fprintf(stderr, "[ERROR] Could not set position: %s\n", SDL_GetError());
      SDL_DestroySurface(surface);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 1;
    }

    if (!SDL_UpdateWindowSurface(window)) {
      fprintf(stderr, "[ERROR] Could not update surface: %s\n", SDL_GetError());
      SDL_DestroySurface(surface);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 1;
    }

    if (!SDL_ShowWindow(window)) {
      fprintf(stderr, "[ERROR] Could not show window: %s\n", SDL_GetError());
      SDL_DestroySurface(surface);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 1;
    }
  }

  SDL_DestroySurface(surface);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
