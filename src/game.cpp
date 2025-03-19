#include <iostream>
#include "RenderWindow.hpp"
#include "Entity.hpp"

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) > 0)
		std::cout << "SDL_Init has failed, SDL ERROR: " << SDL_GetError();
	if (!(IMG_Init(IMG_INIT_PNG)))
		std::cout << "IMG_Init has failed, SDL ERROR: " << SDL_GetError();
	
	RenderWindow window("GAME v1.0", 1080, 600);
	
	const char *assetPath = "../assets/";
	// CHANGE FILEPATH FOR TEXTURE LOADER
	SDL_Texture* grassTexture = window.loadTexture((std::string(assetPath) + "images/ground_grass_1.png").c_str());

	Entity entities[3] = { Entity(0, 568,grassTexture),
						   Entity(32,568,grassTexture),
						   Entity(64,568,grassTexture) };

	#ifdef CI_BUILD
    SDL_Delay(5000);
    SDL_Quit();
    return 0;
    #endif
	
	bool gameRunning = true;

	SDL_Event event;

	while (gameRunning)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				gameRunning = false;
			}
		}
		window.clear();
		for (int i = 0; i < 3; i++)
		{
			window.render(entities[i]);
		}
		window.display();
	}

	window.~RenderWindow();
	SDL_Quit();

	return 0;
}