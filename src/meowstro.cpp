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
		SDL_Texture* logoTexture = window.loadTexture("../assets/images/menu_cat.png");
		SDL_Texture *selectedTexture = window.loadTexture("../assets/images/select_cat.png");
		Entity logoCat(660, 200, logoTexture);
		Sprite selectCat(760, 500, selectedTexture, 1, 1);
		//Entity menu = Entity(0, 0, window.loadTexture("../assets/images/Ocean.png"));

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
			// if represents selected option render location
			if (option)
				selectCat.setLoc(760, 775);
			else
				selectCat.setLoc(760, 600);
			window.render(selectCat);
			window.render(logoCat);
			window.display();
		}
	}
	
	const int NUM_FISH_TEXTURES = 5;
	SDL_Texture* fishTextures[NUM_FISH_TEXTURES];
	fishTextures[0] = window.loadTexture("../assets/images/blue_fish.png");
	fishTextures[1] = window.loadTexture("../assets/images/green_fish.png");
	fishTextures[2] = window.loadTexture("../assets/images/gold_fish.png");
	SDL_Texture* oceanTexture = window.loadTexture("../assets/images/Ocean.png");
	SDL_Texture* boatTexture = window.loadTexture("../assets/images/boat.png");
	SDL_Texture* fisherTexture = window.loadTexture("../assets/images/fisher.png");
	Sprite fish[NUM_FISH_TEXTURES] = { Sprite(1620, 720, fishTextures[0], 1, 6), Sprite(1380, 720, fishTextures[1], 1, 6), Sprite(1140, 720, fishTextures[2], 1, 6), Sprite(900, 720, fishTextures[1], 1, 6), Sprite(660, 720, fishTextures[2], 1, 6) };
	Sprite boat(150, 350, boatTexture, 1, 1);
	Sprite fisher(300, 200, fisherTexture, 1, 2);
	Entity ocean(0, 0, oceanTexture);

	int thrownTimer = 0;
	int bob = 0;
	int sway = 0;
	bool left = false;
	bool thrown = false;

	Audio player;
	player.playBackgroundMusic("../assets/audio/song_for_meowstro.mp3");
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
				case SDLK_SPACE:
					if (!thrown)
					{
						thrown = true;
						thrownTimer = 3;
					}
					break;
				}
			}
		}
		window.clear();
		window.render(ocean);
		static float timeCounter = 0.0f;
		timeCounter += 0.05f;

		for (int i = 0; i < NUM_FISH_TEXTURES; i++)
		{
			sway = static_cast<int>(sin(timeCounter + i) * 1.25);
			bob = static_cast<int>(cos(timeCounter + i) * 1.25);

			fish[i].setLoc(fish[i].getX() + sway, fish[i].getY() + bob);
		}

		// sway boat and cat
		boat.setLoc(boat.getX() + sway, boat.getY() + bob);
		fisher.setLoc(fisher.getX() + sway, fisher.getY() + bob);
		if (thrown)
		{
			fisher.setFrame(1, 2); // Show frame 2
			thrownTimer--;

			if (thrownTimer <= 0)
			{
				thrown = false;
				fisher.setFrame(1, 1); // Return to frame 1
			}
		}
		else
		{
			fisher.setFrame(1, 1); // Idle on frame 1
		}

		// render fish
		for (int i = 0; i < NUM_FISH_TEXTURES; i++)
		{
			window.render(fish[i]++);
			if (fish[i].getCol() == 4)
				fish[i].resetFrame();
		}

		window.render(boat);
		window.render(fisher);

		window.display();
		SDL_Delay(50);
	}
	player.stopBackgroundMusic();
	window.~RenderWindow();
	player.~Audio();
	SDL_Quit();

	return 0;
}
