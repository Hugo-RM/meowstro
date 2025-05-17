#include <iostream>
#include <string>
#include "RenderWindow.hpp"
#include "AudioLogic.hpp"
#include "Entity.hpp"
#include "Sprite.hpp"
#include "Audio.hpp"
#include "Font.hpp"


int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) > 0)
		std::cout << "SDL_Init has failed, SDL ERROR: " << SDL_GetError();
	if (!(IMG_Init(IMG_INIT_PNG)))
		std::cout << "IMG_Init has failed, SDL ERROR: " << SDL_GetError();
	if (TTF_Init() == -1)
		std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;

	RenderWindow window("Meowstro v1.1", 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

#ifdef CI_BUILD
	SDL_Delay(3000);
	SDL_Quit();
	return 0;
#endif

	bool gameRunning = true;
	SDL_Event event;
	const char* comicSans = "../assets/fonts/Comic Sans MS.ttf";
	// menu 
	{
		bool onMenu = true;
		bool option = false;
		Font logoFont, startFont, quitFont;
		logoFont.load(comicSans, 75);
		startFont.load(comicSans, 55);
		quitFont.load(comicSans, 65);
		SDL_Texture* quitTexture = quitFont.renderText(window.getRenderer(), "QUIT", { 255, 255, 100, 255 });
		SDL_Texture* startTexture = startFont.renderText(window.getRenderer(), "START", { 255, 255, 100, 255 });
		SDL_Texture* logoTexture = logoFont.renderText(window.getRenderer(), "MEOWSTRO", { 255, 255, 100, 255 });
		SDL_Texture* logoCatTexture = window.loadTexture("../assets/images/menu_cat.png");
		SDL_Texture* selectedTexture = window.loadTexture("../assets/images/select_cat.png");
		Entity quit(850, 800, quitTexture);
		Entity logo(725, 350, logoTexture);
		Entity start(850, 625, startTexture);
		Entity logoCat(660, 200, logoCatTexture);
		Sprite selectCat(760, 500, selectedTexture, 1, 1);


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
			window.render(logo);
			window.render(start);
			window.render(quit);
			window.display();
		}
		logoFont.unload();
		startFont.unload();
		quitFont.unload();
	}
	
	const int NUM_FISH_TEXTURES = 5;
	int locationsX[NUM_FISH_TEXTURES] = { 660, 900, 1140, 1380, 1620 };
	SDL_Texture* fishTextures[NUM_FISH_TEXTURES];
	fishTextures[0] = window.loadTexture("../assets/images/blue_fish.png");
	fishTextures[1] = window.loadTexture("../assets/images/green_fish.png");
	fishTextures[2] = window.loadTexture("../assets/images/gold_fish.png");
	SDL_Texture* oceanTexture = window.loadTexture("../assets/images/Ocean.png");
	SDL_Texture* boatTexture = window.loadTexture("../assets/images/boat.png");
	SDL_Texture* fisherTexture = window.loadTexture("../assets/images/fisher.png");
	SDL_Texture* hookTexture = window.loadTexture("../assets/images/hook.png");
	Sprite fish[NUM_FISH_TEXTURES] = { Sprite(locationsX[0], 720, fishTextures[0], 1, 6),
									   Sprite(locationsX[1], 720, fishTextures[1], 1, 6),
									   Sprite(locationsX[2], 720, fishTextures[2], 1, 6), 
									   Sprite(locationsX[3], 720, fishTextures[1], 1, 6), 
									   Sprite(locationsX[4], 720, fishTextures[2], 1, 6) };

	Sprite boat(150, 350, boatTexture, 1, 1);
	Sprite fisher(300, 200, fisherTexture, 1, 2);
	Sprite hook(400, 200, hookTexture, 1, 1);
	Entity ocean(0, 0, oceanTexture);

	int thrownTimer = 0;
	int bob = 0;
	int sway = 0;
	bool left = false;
	bool thrown = false;
	bool isThrowing = false;
	int throwDuration = 300; // in milliseconds (0.5 seconds)
	Uint32 throwStartTime = 0;
	int hookStartX, hookStartY;
	int hookTargetX = 650;
	int hookTargetY = 625;


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
					if (!isThrowing)
					{
						thrown = true;
						isThrowing = true;
						throwStartTime = SDL_GetTicks();
						hookStartX = hook.getX();
						hookStartY = hook.getY();
						thrownTimer = 3;
					}
					break;
				}
			}
		}
		window.clear();
		window.render(ocean);
		static double timeCounter = 0.0f;
		timeCounter += 0.05;

		for (int i = 0; i < NUM_FISH_TEXTURES; i++)
		{
			sway = static_cast<int>(sin(timeCounter + i) * 1.25);
			bob = static_cast<int>(cos(timeCounter + i) * 1.25);

			fish[i].setLoc(fish[i].getX() + sway, fish[i].getY() + bob);
		}

		// sway boat and cat
		hook.setLoc(hook.getX() + sway, hook.getY() + bob);
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

		if (isThrowing)
		{
			Uint32 now = SDL_GetTicks();
			Uint32 elapsed = now - throwStartTime;

			float progress = static_cast<float>(elapsed) / throwDuration;
			if (progress >= 1.0f)
			{
				progress = 1.0f;
				isThrowing = false;
			}

			int newX = static_cast<int>(hookStartX + (hookTargetX - hookStartX) * progress);
			int newY = static_cast<int>(hookStartY + (hookTargetY - hookStartY) * progress);

			hook.setLoc(newX, newY);
		}
		else
		{
			// Sway + bob when not throwing
			hook.setLoc(hook.getX() + sway, hook.getY() + bob);
		}


		// render fish
		for (int i = 0; i < NUM_FISH_TEXTURES; i++)
		{
			window.render(fish[i]++);
			if (fish[i].getCol() == 4)
				fish[i].resetFrame();
		}

		window.render(boat);
		window.render(hook);
		window.render(fisher);

		window.display();
		SDL_Delay(75);
	}
	player.stopBackgroundMusic();
	window.~RenderWindow();
	player.~Audio();
	TTF_Quit();
	SDL_Quit();

	return 0;
}
