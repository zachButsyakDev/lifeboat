#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <SDL2_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
// main function just handles sdl2, windows, runtime loop

int main(int argc, char *argv[]) {

  SDL_Window *window; // declare window pointer

  SDL_Init(SDL_INIT_VIDEO);

  window =
      SDL_CreateWindow("Lifeboat", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);

  if (window == NULL) {
    printf("SDL Could Not Be Initialized.\n");
    return 1;
  } else {
    printf("SDL is initialized!\n");
  }

  bool isRunning = true;

  while (isRunning) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        isRunning = false;
      }
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
