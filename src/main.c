#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

float scrollOffset = 0.0f;       // Where we are currently looking
float targetScrollOffset = 0.0f; // Where we WANT to be (0, 1280, 2560...)
float lerpSpeed = 0.1f;          // How fast it "slides" (0.1 is 10% per frame)

enum GameEnum { OOT, MM, MK };

struct game {
  enum GameEnum game;
  char logoDir[48], bgDir[48], installDir[48], romDir[48];
  int logoWidth, logoHeight;
  SDL_Texture *logo;
  SDL_Texture *bg;
  char musicDir[48], effectDir[48];
};

struct game games[3] = {
    {OOT, "../public/images/oot_logo.png",
     "../public/images/oot_bg_vignette.png", "/Applications/soh.app/", "Empty",
     0, 0, 0, 0, "../public/audio/oot_theme.wav",
     "../public/audio/ganon_laugh.wav"},
    {MM, "../public/images/mm_logo.png", "../public/images/mm_bg.png",
     "/Applications/2s2h.app/", "Empty", 0, 0, 0, 0,
     "../public/audio/majoras_theme.wav", "../public/audio/majora_laugh.wav"},
    {MK, "../public/images/mario_kart.png",
     "../public/images/oot_bg_vignette.png", "Empty", "Empty", 0, 0, 0, 0,
     "../public/audio/oot_theme.wav", "../public/audio/ganon_laugh.wav"}};

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

  // start SDL with audio support
  if (SDL_Init(SDL_INIT_AUDIO) == -1) {
    printf("SDL_Init: %s\n", SDL_GetError());
    exit(1);
  } else if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
    printf("Mix_OpenAudio: %s\n", Mix_GetError());
    exit(2);
  } else {
    printf("SDL Audio successfully initialized.");
  }

  Mix_AllocateChannels(6);
  Mix_Chunk *songs[3];
  for (int i = 0; i < 3; i++) {
    // load sample.wav in to sample
    songs[i] = Mix_LoadWAV(games[i].musicDir);
    if (!songs[i]) {
      printf("Mix_LoadWAV: %s\n", Mix_GetError());
      // handle error
    }
  }
  Mix_Chunk *effects[3];
  for (int i = 0; i < 3; i++) {
    // load sample.wav in to sample
    effects[i] = Mix_LoadWAV(games[i].effectDir);
    if (!effects[i]) {
      printf("Mix_LoadWAV: %s\n", Mix_GetError());
      // handle error
    }
  }

  if (Mix_PlayChannel(0, songs[0], 0) == -1) {
    printf("Mix_PlayChannel: %s\n", Mix_GetError());
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(renderer, 1280, 720);

  int imgFlags = IMG_INIT_PNG;
  int imgInitStatus = IMG_Init(imgFlags);

  if ((imgInitStatus & imgFlags) != imgFlags) {
    printf("SDL Image Format Not Available!");
  }

  for (int i = 0; i < 3; i++) {
    SDL_Surface *image;
    SDL_Surface *bgImage;
    image = IMG_Load(games[i].logoDir);
    bgImage = IMG_Load(games[i].bgDir);

    if (!image || !bgImage) {
      printf("Image not loaded!");
      return 1;
    }
    // Store the original size
    games[i].logoWidth = image->w;
    games[i].logoHeight = image->h;

    games[i].logo = SDL_CreateTextureFromSurface(renderer, image);
    games[i].bg = SDL_CreateTextureFromSurface(renderer, bgImage);
    SDL_FreeSurface(image);
    SDL_FreeSurface(bgImage);
  }

  SDL_Event event;

  bool isRunning = true;

  int activeGame = 0;

  while (isRunning) {
    Uint32 time = SDL_GetTicks();
    scrollOffset += (targetScrollOffset - scrollOffset) * lerpSpeed;
    SDL_RenderClear(renderer);
    // --- DRAW BACKGROUNDS FIRST ---
    for (int i = 0; i < 3; i++) {
      SDL_Rect bgRect;
      bgRect.w = 1280;
      bgRect.h = 720;
      // Backgrounds move at the exact same rate as the logos
      bgRect.x = (i * 1280) - (int)scrollOffset;
      bgRect.y = 0;

      // Only draw background if it's visible
      if (bgRect.x + bgRect.w > 0 && bgRect.x < 1280) {
        SDL_RenderCopy(renderer, games[i].bg, NULL, &bgRect);

        // OPTIONAL: Add a dark dimming effect so the logo stands out
        // Comment these 3 lines out if you want the BG at full brightness
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100); // 100/255 darkness
        SDL_RenderFillRect(renderer, &bgRect);
      }
    }
    // 3. Draw ALL games relative to the scroll
    for (int i = 0; i < 3; i++) {
      SDL_Rect logoRect;
      float scale = 0.5f;
      logoRect.w = (int)(games[i].logoWidth * scale);
      logoRect.h = (int)(games[i].logoHeight * scale);

      // X Position calculation:
      // (i * 1280) puts them in a line.
      // - scrollOffset moves the whole "line" left or right.
      // + (1280 - logoRect.w)/2 centers the individual logo in its slot.
      logoRect.x = (i * 1280) - (int)scrollOffset + (1280 - logoRect.w) / 2;
      logoRect.y = (720 - logoRect.h) / 2;

      // Only draw if it's actually on screen (optimization)
      if (logoRect.x + logoRect.w > 0 && logoRect.x < 1280) {
        SDL_RenderCopy(renderer, games[i].logo, NULL, &logoRect);
      }
    }
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
          if (event.key.keysym.sym == SDLK_RIGHT) {
            if (activeGame < 2) {
              activeGame++;
              targetScrollOffset += 1280.0f;
              Mix_HaltChannel(-1);
              if (Mix_PlayChannel(activeGame, songs[activeGame], 0) == -1) {
                printf("Mix_PlayChannel: %s\n", Mix_GetError());
              }
            }
          }
          if (event.key.keysym.sym == SDLK_LEFT) {
            if (activeGame != 0) {
              activeGame--;
              targetScrollOffset -= 1280.0f;
              Mix_HaltChannel(-1);
              if (Mix_PlayChannel(activeGame, songs[activeGame], 0) == -1) {
                printf("Mix_PlayChannel: %s\n", Mix_GetError());
              }
            }
          }
          if (event.key.keysym.sym == SDLK_x) {

            Mix_HaltChannel(-1);
            if (Mix_PlayChannel(activeGame + 4, effects[activeGame], 0) == -1) {
              printf("Mix_PlayChannel: %s\n", Mix_GetError());
            }
            sleep(2);
            char command[1024];
            sprintf(command, "open \"%s\"", games[activeGame].installDir);
            printf("Launching %s...\n", games[activeGame].installDir);
            system(command);
            isRunning = false;
          }
        }
        break;
      case SDL_QUIT:
        isRunning = false;
        break;
      }
    }
    SDL_Delay(16);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
