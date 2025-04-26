#pragma once
#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_mixer.h>

class Audio {
public:
	Audio();
	~Audio();
	void playBackgroundMusic(const std::string& filePath);
	void stopBackgroundMusic();
};
