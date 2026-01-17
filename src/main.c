#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
// main function just handles sdl2, windows, runtime loop

int main(int argc, char *argv[]) {

  SDL_Window *window; // declare window pointer

  SDL_Init(SDL_INIT_VIDEO);

  window =
      SDL_CreateWindow("Lifeboat", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);

  if (window == NULL) {
    printf("SDL Could Not Be Initialized.\n");
    return 1;
  } else {
    printf("SDL is initialized!\n");
  }

  SDL_Event event;
  int quit = 0;
  while (!quit) {
    // This is the "Pump" that makes the window appear on Mac
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        quit = 1;
      }
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
