#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
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

  int imgFlags = IMG_INIT_PNG;
  int imgInitStatus = IMG_Init(imgFlags);

  if ((imgInitStatus & imgFlags) != imgFlags) {
    printf("SDL Image Format Not Available!");
  }

  SDL_Surface *image;
  image = IMG_Load("../public/images/mm_logo.png");

  if (!image) {
    printf("Image not loaded!");
    return 1;
  }
  // Store the original size
  int imgW = image->w;
  int imgH = image->h;

  SDL_Texture *png = SDL_CreateTextureFromSurface(renderer, image);
  SDL_FreeSurface(image);

  SDL_Event event;

  bool isRunning = true;

  while (isRunning) {
    // 1. Clear the screen (Blue background)
    SDL_RenderClear(renderer);

    SDL_Rect logoRect;
    float scale =
        0.5f; // Adjust this to make the logo bigger or smaller (0.5 = 50% size)

    logoRect.w = (int)(imgW * scale);
    logoRect.h = (int)(imgH * scale);

    // Center it on your 1280x720 logical screen
    logoRect.x = (1280 - logoRect.w) / 2;
    logoRect.y = 50;

    SDL_RenderCopy(renderer, png, NULL, &logoRect);
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
