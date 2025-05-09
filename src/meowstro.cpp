#include <iostream>
//#include <Windows.h>
#include <string>
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Audio.hpp"

int main(int argc, char *args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) > 0)
		std::cout << "SDL_Init has failed, SDL ERROR: " << SDL_GetError();
	if (!(IMG_Init(IMG_INIT_PNG)))
		std::cout << "IMG_Init has failed, SDL ERROR: " << SDL_GetError();

	//SetProcessDPIAware();
	//SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");
	//GetSystemMetrics(SM_CXSCREEN);
	//GetSystemMetrics(SM_CYSCREEN);
	RenderWindow window("Meowstro v1.1", 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	
#ifdef CI_BUILD
	SDL_Delay(3000);
	SDL_Quit();
	return 0;
#endif
	
	bool gameRunning = true;
	SDL_Event event;
	//{
	//	bool onMenu = true;
	//	short int option = 0;
	//	SDL_Texture* selected = window.loadTexture("../assets/images/selected.png");
	//	Entity menu = Entity(0, 0, window.loadTexture("../assets/images/Ocean.png"));

	//	while (onMenu)
	//	{
	//		while (SDL_PollEvent(&event))
	//		{
	//			switch (event.type)
	//			{
	//			case SDL_QUIT:
	//			{
	//				onMenu = false;
	//				gameRunning = false;
	//				break;
	//			}
	//			case SDL_KEYDOWN:
	//			{
	//				switch (event.key.keysym.sym) // input
	//				{
	//				case SDLK_ESCAPE:
	//				{
	//					onMenu = false;
	//					gameRunning = false;
	//					break;
	//				}
	//				case SDLK_SPACE:
	//				{
	//					onMenu = false;
	//					gameRunning = false;
	//					break;
	//				}
	//				}
	//			}
	//			}
	//		}
	//		window.clear();
	//		window.render(menu);
	//		switch (option)
	//		{
	//		case 0:
	//			break;
	//		case 1:
	//			break;
	//		case 2:
	//			break;
	//		default:
	//			break;
	//		}
	//		window.display();

	//	}
	//}
	
	const int NUM_FISH_TEXTURES = 3;
	SDL_Texture* fishTextures[NUM_FISH_TEXTURES];
	fishTextures[0] = window.loadTexture("../assets/images/blue_fish.png");
	fishTextures[1] = window.loadTexture("../assets/images/green_fish.png");
	fishTextures[2] = window.loadTexture("../assets/images/gold_fish.png");

	Entity fish[NUM_FISH_TEXTURES] = { Entity(0, 0, fishTextures[0]), Entity(0, 128, fishTextures[1]), Entity(0, 256, fishTextures[2]) };

	Audio player;
	player.playBackgroundMusic("../assets/audio/mymarch.mp3");
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
		for (int i = 0; i < NUM_FISH_TEXTURES; i++)
		{
			window.render(fish[i]);
		}
	//	window.render(background);
	//	for (int i = 0; i < 8; i++)
	//	{
	//		window.render(fishEntities[i]);
	//	}
		
		window.display();
		
	}
	player.stopBackgroundMusic();
	window.~RenderWindow();
	player.~Audio();
	SDL_Quit();


	return 0;
}
