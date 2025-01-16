#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  SDL_WindowFlags flags = SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS;
  int posX = SDL_WINDOWPOS_CENTERED, posY = SDL_WINDOWPOS_CENTERED;
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

  // using this ID will crash the programm, even though it is '1' when printed
  //
  // SDL_DisplayID displayId = SDL_GetDisplayForWindow(window);
  // if (displayId == 0) {
  //   fprintf(stderr, "[ERROR] Could not retrieve display: %s\n",
  //   SDL_GetError()); SDL_DestroySurface(surface); SDL_DestroyWindow(window);
  //   SDL_Quit();
  //   return 1;
  // }

  const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(1);
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

  bool running = true;
  while (running) {
    SDL_Event *event;
    while (SDL_PollEvent(event)) {
      if (event->type == SDL_EVENT_QUIT) {
        running = false;
      } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event->button.button == SDL_BUTTON_MIDDLE) {
          running = false;
        } else if (event->button.button == SDL_BUTTON_LEFT) {
          posX = displayMode->w / 2 - width / 2 - 100;
        } else if (event->button.button == SDL_BUTTON_RIGHT) {
          posX = displayMode->w / 2 - width / 2 + 100;
        }
      }
    }

    if (!SDL_SetWindowPosition(window, posX, posY)) {
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
  }

  SDL_DestroySurface(surface);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
