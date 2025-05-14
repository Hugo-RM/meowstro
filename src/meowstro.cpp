#include <iostream>
//#include <Windows.h>
#include <string>
#include "RenderWindow.hpp"
#include "Sprite.hpp"
#include "Entity.hpp"
#include "Audio.hpp"
#include <filesystem>

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) > 0)
		std::cout << "SDL_Init has failed, SDL ERROR: " << SDL_GetError();
	if (!(IMG_Init(IMG_INIT_PNG)))
		std::cout << "IMG_Init has failed, SDL ERROR: " << SDL_GetError();

	RenderWindow window("Meowstro v1.1", 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

#ifdef CI_BUILD
	SDL_Delay(3000);
	SDL_Quit();
	return 0;
#endif

	bool gameRunning = true;
	SDL_Event event;
	{
		bool onMenu = true;
		bool option = false;
		SDL_Texture* selectedTexture = window.loadTexture("C:/Users/Hugo/Documents/all-da-code/school-assignments/CSS-2/meowstro/assets/images/select_cat.png");
		Sprite select_cat(760, 500, selectedTexture, 1, 1);
		Entity menu = Entity(0, 0, window.loadTexture("../assets/images/Ocean.png"));

		while (onMenu)
		{
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
					case SDL_KEYDOWN:
					{
						switch (event.key.keysym.sym) // input
						{
							case SDLK_ESCAPE:
							{
								onMenu = false;
								gameRunning = false;
								break;
							}
							case SDLK_SPACE:
							{
								onMenu = false;
								gameRunning = (option) ? false : true; // op0 = start op1 = exit
								break;
							}
							case SDLK_UP:
							case SDLK_DOWN:
							{
								if (event.key.repeat == 0) // Only first press, not repeated holding
								{
									option = (option) ? false : true;
								}
								break;
							}
						}
					}
				}
			}
			window.clear();
			window.render(menu);
			// if represents selected option render location
			if (option)
				select_cat.setLoc(760, 775);
			else
				select_cat.setLoc(760, 600);
			window.render(select_cat);
			window.display();
		}
	}
	
	const int NUM_FISH_TEXTURES = 3;
	SDL_Texture* fishTextures[NUM_FISH_TEXTURES];
	fishTextures[0] = window.loadTexture("../assets/images/blue_fish.png");
	fishTextures[1] = window.loadTexture("../assets/images/green_fish.png");
	fishTextures[2] = window.loadTexture("../assets/images/gold_fish.png");

	Sprite fish[NUM_FISH_TEXTURES] = { Sprite(0, 0, fishTextures[0], 1, 1), Sprite(0, 128, fishTextures[1], 1, 1), Sprite(0, 256, fishTextures[2], 1, 1) };

	Audio player;
	player.playBackgroundMusic("../assets/audio/mymarch.mp3");
	window.clear();
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
