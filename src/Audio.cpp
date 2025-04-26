#include "Audio.hpp"
#include <string>
#include <iostream>


Audio::Audio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL audio: " << SDL_GetError() << std::endl;
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return;
    }

  /*  Mix_AllocateChannels(16);*/ // Optional: Set up channels
}
Audio::~Audio() {
    Mix_CloseAudio();
    SDL_Quit();
}
void Audio::playBackgroundMusic(const std::string& filePath) {
    Mix_Music* bgMusic = Mix_LoadMUS(filePath.c_str());
    if (!bgMusic) {
        std::cerr << "Failed to load music file: " << Mix_GetError() << std::endl;
        return;
    }

    if (Mix_PlayMusic(bgMusic, -1) < 0) {
        std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
        Mix_FreeMusic(bgMusic);
        return;
    }
}
void Audio::stopBackgroundMusic() {
    Mix_HaltMusic();
}
