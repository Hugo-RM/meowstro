#include <iostream>
#include <Windows.h>
#include <string>
#include <SDL_ttf.h>
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Audio.hpp"
#include "AudioLogic.hpp"
#include <filesystem>

int main(int argc, char *args[])
{

	if (SDL_Init(SDL_INIT_VIDEO) > 0)
		std::cout << "SDL_Init has failed, SDL ERROR: " << SDL_GetError();
	if (!(IMG_Init(IMG_INIT_PNG)))
		std::cout << "IMG_Init has failed, SDL ERROR: " << SDL_GetError();

	SetProcessDPIAware();
	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");

	int nativeWidth = GetSystemMetrics(SM_CXSCREEN);
	int nativeHeight = GetSystemMetrics(SM_CYSCREEN);

	int windowWidth = nativeWidth * 0.9;
	int windowHeight = nativeHeight * 0.9;

	RenderWindow window("Meowstro v1.0", windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	
#ifdef CI_BUILD
	SDL_Delay(3000);
	SDL_Quit();
	return 0;
#endif
	
	const int NUM_FISH_TEXTURES = 9;
	SDL_Texture* fishTextures[NUM_FISH_TEXTURES];
	
	//fishTextures[0] = window.loadTexture("../assets/images/blue-fish/fish-1.png");
	//fishTextures[1] = window.loadTexture("../assets/images/blue-fish/fish-2.png");
	//fishTextures[2] = window.loadTexture("../assets/images/blue-fish/fish-3.png");
	//fishTextures[3] = window.loadTexture("../assets/images/green-fish/fish-1.png");
	//fishTextures[4] = window.loadTexture("../assets/images/green-fish/fish-2.png");
	//fishTextures[5] = window.loadTexture("../assets/images/green-fish/fish-3.png");
	//fishTextures[6] = window.loadTexture("../assets/images/gold-fish/fish-1.png");
	//fishTextures[7] = window.loadTexture("../assets/images/gold-fish/fish-2.png");
	//fishTextures[8] = window.loadTexture("../assets/images/gold-fish/fish-3.png");

	//Entity fishEntities[8] = { Entity(180, 570, fishTextures[0]),
	//						   Entity(408, 570, fishTextures[1]),
	//						   Entity(636, 570, fishTextures[2]),
	//						   Entity(864, 570, fishTextures[3]),
	//						   Entity(1092, 570, fishTextures[4]),
	//						   Entity(1092, 410, fishTextures[5]),
	//						   Entity(1092, 250, fishTextures[6]),
	//						   Entity(1092, 90, fishTextures[7]) };
	bool gameRunning = true;
	static int bpm = 147;
	SDL_Event event;
	Audio player;
	AudioLogic gamePlay;

	std::vector<bool> noteHitFlags(49, false);
	ExpectedHit userHit;

	int hit_counter = 0;
	int songStartTime = SDL_GetTicks();
	player.playBackgroundMusic("C:\\Users\\xxsha\\source\\repos\\Hugo-RM\\meowstro\\assets\\audio\\song_for_meowstro.mp3");

	while (gameRunning) {
		int currentTime = SDL_GetTicks() - songStartTime;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) {
				gameRunning = false;
				break;
			}
			else if (event.type == SDL_KEYDOWN || event.key.keysym.sym == SDLK_SPACE) {
				// Check for upcoming note in window
				for (int i = 0; i < 49; ++i) {
					if (noteHitFlags[i]) continue; // already hit

					ExpectedHit expected = gamePlay.getHit(i);
					int expectedMs = gamePlay.convertBSTtoMs(expected, bpm);
					int delta = abs(currentTime - expectedMs);

					if (delta <= gamePlay.getGOODW()) { // If in hit window
						ExpectedHit userHit;
						gamePlay.converteToBST(currentTime, bpm, userHit);
						gamePlay.checkHit(expected, userHit, bpm);
						noteHitFlags[i] = true; // Mark as hit
						break;
						std::cout << "GOOD Hit" << std::endl;
					}
				}
			}
		}

		// Automatic MISS handling: if note passed its window and was not hit
		for (int i = 0; i < 49; ++i) {
			if (noteHitFlags[i]) continue;

			ExpectedHit note = gamePlay.getHit(i);
			int noteTime = gamePlay.convertBSTtoMs(note, bpm);
			if (currentTime > noteTime + gamePlay.getGOODW()) {
				std::cout << "Miss" << std::endl;
				noteHitFlags[i] = true;
			}
		}

		// Redraw and update
		window.clear();
		window.display();
	}



	player.stopBackgroundMusic();
	window.~RenderWindow();
	player.~Audio();
	SDL_Quit();


	return 0;
}

