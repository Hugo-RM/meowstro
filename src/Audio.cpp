#include "Audio.hpp"
#include "Logger.hpp"
#include <string>
#include <iostream>


Audio::Audio() : bgMusic(nullptr), m_valid(false) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        Logger::logSDLError(LogLevel::ERROR, "Failed to initialize SDL audio");
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        Logger::logSDLMixerError(LogLevel::ERROR, "Failed to initialize SDL_mixer");
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }
    
    m_valid = true;
    Logger::info("Audio system initialized successfully");
}
Audio::~Audio() {
    if (bgMusic) {
        Mix_FreeMusic(bgMusic);
        bgMusic = nullptr;
    }
    Mix_CloseAudio();
}
void Audio::playBackgroundMusic(const std::string& filePath) {
    if (!m_valid) {
        Logger::error("Audio::playBackgroundMusic called on invalid Audio system");
        return;
    }
    
    if (filePath.empty()) {
        Logger::error("Audio::playBackgroundMusic called with empty file path");
        return;
    }
    
    // Free any existing music first
    if (bgMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgMusic);
        bgMusic = nullptr;
    }
    
    bgMusic = Mix_LoadMUS(filePath.c_str());
    if (!bgMusic) {
        Logger::logSDLMixerError(LogLevel::ERROR, "Failed to load music file: " + filePath);
        return;
    }
    
    if (Mix_PlayMusic(bgMusic, 0) < 0) {
        Logger::logSDLMixerError(LogLevel::ERROR, "Failed to play music: " + filePath);
        Mix_FreeMusic(bgMusic);
        bgMusic = nullptr;
        return;
    }
    
    Logger::info("Started playing background music: " + filePath);
}
void Audio::stopBackgroundMusic() {
    if (!m_valid) {
        Logger::warning("Audio::stopBackgroundMusic called on invalid Audio system");
        return;
    }
    
    Mix_HaltMusic();
    if (bgMusic) {
        Mix_FreeMusic(bgMusic);
        bgMusic = nullptr;
        Logger::info("Stopped background music");
    }
}
