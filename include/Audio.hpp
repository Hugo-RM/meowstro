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
	bool isValid() const { return m_valid; }
	
	// Get precise audio position for beat synchronization
	double getMusicPositionMs() const;

private:
	Mix_Music* bgMusic;
	bool m_valid;
};

