#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

float scrollOffset = 0.0f;
float targetScrollOffset = 0.0f;
float lerpSpeed = 0.1f;

enum GameEnum { OOT, MM, MK };

struct Game {
  GameEnum type;
  std::string logoDir, bgDir, installDir, romDir;
  int logoWidth, logoHeight;
  SDL_Texture *logo = nullptr;
  SDL_Texture *bg = nullptr;
  std::string musicDir, effectDir;
};

struct AudioConfig {
  std::string audioOnDir, audioOffDir;
};

// Use std::vector for flexible management
std::vector<Game> games = {
    {OOT, "../public/images/oot_logo.png",
     "../public/images/oot_bg_vignette.png", "/Applications/soh.app/", "Empty",
     0, 0, nullptr, nullptr, "../public/audio/oot_theme.wav",
     "../public/audio/ganon_laugh.wav"},
    {MM, "../public/images/mm_logo.png", "../public/images/mm_bg.png",
     "/Applications/2s2h.app/", "Empty", 0, 0, nullptr, nullptr,
     "../public/audio/majoras_theme.wav", "../public/audio/majora_laugh.wav"},
    {MK, "../public/images/mario_kart.png",
     "../public/images/oot_bg_vignette.png", "Empty", "Empty", 0, 0, nullptr,
     nullptr, "../public/audio/oot_theme.wav",
     "../public/audio/ganon_laugh.wav"}};

AudioConfig audioController = {"../public/icons/volume.png",
                               "../public/icons/muted.png"};

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(
      "Lifeboat", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!window)
    return 1;

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
    std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
    return 2;
  }

  Mix_AllocateChannels(6);
  std::vector<Mix_Chunk *> songs;
  std::vector<Mix_Chunk *> effects;

  for (const auto &g : games) {
    songs.push_back(Mix_LoadWAV(g.musicDir.c_str()));
    effects.push_back(Mix_LoadWAV(g.effectDir.c_str()));
  }

  Mix_PlayChannel(0, songs[0], 0);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(renderer, 1280, 720);

  IMG_Init(IMG_INIT_PNG);

  for (auto &g : games) {
    SDL_Surface *tempLogo = IMG_Load(g.logoDir.c_str());
    SDL_Surface *tempBG = IMG_Load(g.bgDir.c_str());

    if (tempLogo && tempBG) {
      g.logoWidth = tempLogo->w;
      g.logoHeight = tempLogo->h;
      g.logo = SDL_CreateTextureFromSurface(renderer, tempLogo);
      g.bg = SDL_CreateTextureFromSurface(renderer, tempBG);
      SDL_FreeSurface(tempLogo);
      SDL_FreeSurface(tempBG);
    }
  }

  SDL_Event event;
  bool isRunning = true;
  int activeGame = 0;
  int audioActive = 5;

  while (isRunning) {
    scrollOffset += (targetScrollOffset - scrollOffset) * lerpSpeed;
    SDL_RenderClear(renderer);

    for (int i = 0; i < games.size(); i++) {
      SDL_Rect bgRect = {(i * 1280) - (int)scrollOffset, 0, 1280, 720};
      if (bgRect.x + bgRect.w > 0 && bgRect.x < 1280) {
        SDL_RenderCopy(renderer, games[i].bg, nullptr, &bgRect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
        SDL_RenderFillRect(renderer, &bgRect);
      }
    }

    for (int i = 0; i < games.size(); i++) {
      float scale = 0.5f;
      SDL_Rect logoRect;
      logoRect.w = static_cast<int>(games[i].logoWidth * scale);
      logoRect.h = static_cast<int>(games[i].logoHeight * scale);
      logoRect.x = (i * 1280) - (int)scrollOffset + (1280 - logoRect.w) / 2;
      logoRect.y = (720 - logoRect.h) / 2;

      if (logoRect.x + logoRect.w > 0 && logoRect.x < 1280) {
        SDL_RenderCopy(renderer, games[i].logo, nullptr, &logoRect);
      }
    }

    Mix_Volume(activeGame, audioActive);
    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        isRunning = false;
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_RIGHT:
          if (activeGame < games.size() - 1) {
            activeGame++;
            targetScrollOffset += 1280.0f;
            Mix_HaltChannel(-1);
            Mix_PlayChannel(activeGame, songs[activeGame], 0);
          }
          break;
        case SDLK_LEFT:
          if (activeGame > 0) {
            activeGame--;
            targetScrollOffset -= 1280.0f;
            Mix_HaltChannel(-1);
            Mix_PlayChannel(activeGame, songs[activeGame], 0);
          }
          break;
        case SDLK_a:
          if (audioActive > 0) {
            audioActive = 0;
          } else {
            audioActive = 5;
          }
          break;
        case SDLK_x:
          Mix_HaltChannel(-1);
          Mix_PlayChannel(activeGame + 4, effects[activeGame], 0);
          SDL_Delay(2000); // SDL_Delay is preferred over sleep() in SDL apps
          std::string cmd = "open \"" + games[activeGame].installDir + "\"";
          system(cmd.c_str());
          isRunning = false;
          break;
        }
      }
    }
    SDL_Delay(16);
  }

  // Cleanup (In full C++, you'd use a Destructor for this!)
  for (auto &g : games) {
    SDL_DestroyTexture(g.logo);
    SDL_DestroyTexture(g.bg);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  Mix_CloseAudio();
  SDL_Quit();

  return 0;
}
