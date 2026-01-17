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

  window = SDL_CreateWindow("Lifeboat", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, 960, 540,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  if (window == NULL) {
    printf("SDL Could Not Be Initialized.\n");
    return 1;
  } else {
    printf("SDL is initialized!\n");
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(renderer, 1280, 720);

  bool isRunning = true;

  SDL_Event event;

  while (isRunning) {
    // 1. Clear the screen (Set draw color to "Lifeboat Blue")
    SDL_SetRenderDrawColor(renderer, 0, 105, 148, 255);
    SDL_RenderClear(renderer);

    // 2. Draw something (e.g., a white square)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // Use your LOGICAL constants, not the dynamic window size
    int logicalW = 1280;
    int logicalH = 720;

    SDL_Rect myRect;
    myRect.w = 100;
    myRect.h = 100;

    // Math based on the FIXED logical scale
    myRect.x = (logicalW - myRect.w) / 2; // Always 590
    myRect.y = (logicalH - myRect.h) / 2; // Always 310

    SDL_RenderFillRect(renderer, &myRect);

    // 3. Present the result (This is where the magic happens)
    SDL_RenderPresent(renderer);

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
