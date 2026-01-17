#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
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
      switch (event.type) {
      case SDL_MOUSEMOTION:
        printf("Mouse is moving.\n");
        break;
      case SDL_KEYDOWN:
        const char *keyName = SDL_GetKeyName(event.key.keysym.sym);
        if (event.key.keysym.sym == SDLK_x || event.key.keysym.sym == SDLK_z ||
            event.key.keysym.sym == SDLK_UP ||
            event.key.keysym.sym == SDLK_DOWN ||
            event.key.keysym.sym == SDLK_LEFT ||
            event.key.keysym.sym == SDLK_RIGHT) {
          printf("%s key was pressed.\n", keyName);
        }
        break;
      case SDL_QUIT:
        isRunning = false;
        break;
      }
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
