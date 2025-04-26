#include <iostream>
#include <Windows.h>
#include <string>
#include <SDL_ttf.h>
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Audio.hpp"


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
	//SDL_Texture* fishTextures[NUM_FISH_TEXTURES];

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
	SDL_Event event;
	Audio player;
	player.playBackgroundMusic("C:/Users/xxsha/source/repos/Hugo-RM/meowstro/assets/audio/mymarch.mp3");
	while (gameRunning)
	{
		
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				gameRunning = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					gameRunning = false;
					break;
				}
			}
		}
		window.clear();
		// window.render(background);
		for (int i = 0; i < 8; i++)
		{
	/*		window.render(fishEntities[i]);*/
		}
		
		window.display();
		
	}
	player.stopBackgroundMusic();
	window.~RenderWindow();
	player.~Audio();
	SDL_Quit();


	return 0;
}
