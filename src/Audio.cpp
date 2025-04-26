#include "Audio.hpp"
#include <iostream>
Audio::Audio() {
	myaudio_name = "";
}
Audio::Audio(std::string myaudio_name) {
	this->myaudio_name = myaudio_name;
}
int Audio::AudioPlayer(std::string path_of_audio) {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL initialization failed: %s\n", SDL_GetError());
		return 1;
	}

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_mixer initialization failed: %s\n", Mix_GetError());
		//SDL_Quit();
		return 1;
	}

	Mix_Music* music = Mix_LoadMUS(path_of_audio.c_str()); // Replace with your music file
	if (music == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s\n", Mix_GetError());
		Mix_Quit();
		//SDL_Quit();
		return 1;
	}

	std::cout << "Playing music..." << std::endl;
	if (Mix_PlayMusic(music, -1) == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to play music: %s\n", Mix_GetError());
		Mix_FreeMusic(music);
		Mix_Quit();
		//SDL_Quit();
		return 1;
	}

	// Keep the program running for a while (replace with your game loop or main program logic)
	SDL_Delay(210000); // Play for 10 seconds

	//std::cout << "Fading out music..." << std::endl;
	//Mix_FadeOutMusic(2000);
	//SDL_Delay(2000); // Wait for the fade out

	Mix_FreeMusic(music);
	Mix_Quit();
	//SDL_Quit();
	return 0;
}
