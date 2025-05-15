#include <iostream>
#include <Windows.h>
#include <string>
#include <SDL_ttf.h>
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Audio.hpp"
#include "AudioLogic.hpp"

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

=
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

	std::vector<bool> noteHitFlags(49, false); //This bool checks for the continueity (if the note has passed) regardless of getting hit. Overall helping with syncing

	std::vector<double> noteBeats = { //The vector contains the clicks in miliseconds
	gamePlay.msFromMscs(0,3,46), gamePlay.msFromMscs(0,7,75), gamePlay.msFromMscs(0,9,38), gamePlay.msFromMscs(0,10,61), gamePlay.msFromMscs(0,12,24), gamePlay.msFromMscs(0,13,06),
	gamePlay.msFromMscs(0,13,87), gamePlay.msFromMscs(0,15,30), gamePlay.msFromMscs(0,17,95), gamePlay.msFromMscs(0,20,0), gamePlay.msFromMscs(0,21,22), gamePlay.msFromMscs(0,23,26),
	gamePlay.msFromMscs(0,27,14), gamePlay.msFromMscs(0,28,57), gamePlay.msFromMscs(0,30,40), gamePlay.msFromMscs(0,31,93), gamePlay.msFromMscs(0,32,65), gamePlay.msFromMscs(0,34,69),
	gamePlay.msFromMscs(0,35,91), gamePlay.msFromMscs(0,37,95), gamePlay.msFromMscs(0,41,83), gamePlay.msFromMscs(0,43,26), gamePlay.msFromMscs(0,45,10), gamePlay.msFromMscs(0,46,52),
	gamePlay.msFromMscs(0,48,57)
	};

	player.playBackgroundMusic("../assets/audio/meowstro_short_ver.mp3");
	int songStartTime = SDL_GetTicks(); //Gets current ticks for better

	while (gameRunning) {
		double currentTime = SDL_GetTicks() - songStartTime; //calculates the delay by comparing the current ticks and when the song starts
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) { //Exit key, stop the geames
				gameRunning = false;
				break;
			}
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) { //Space and down arrow are use to hit or make the clicks
				for (int i = 0; i < noteBeats.size(); ++i) {
					if (noteHitFlags[i]) continue;

					double expected = noteBeats[i];
					double delta = fabs(currentTime - expected); //Calculates the gurrent gap for the hit

					if (delta <= gamePlay.getGOOD()) {
						gamePlay.checkHit(expected, currentTime); //This compares the time the SPACE or DOWN was pressed to the time it is requires for the PERFECT or GOOD or Miss
						noteHitFlags[i] = true;
						break;
					}
				}
			}

		} //In the case the SPACEBAR or DOWN arrow was NOT press, it will display miss and will do a similar job as the previous other loop
		for (int i = 0; i < noteBeats.size(); ++i) {
			if (noteHitFlags[i]) continue;

			double noteTime = noteBeats[i];
			if (currentTime > noteTime + gamePlay.getGOOD()) {
				std::cout << "Miss" << std::endl;
				noteHitFlags[i] = true;
			}
		}
		window.clear();
		window.display();
	}
	player.stopBackgroundMusic();
	window.~RenderWindow();
	player.~Audio();
	SDL_Quit();

	return 0;
}
