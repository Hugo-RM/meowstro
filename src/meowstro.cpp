// Names: Hugo, Jaime, Jay, Leo
// Last Modified: 08/30/25
// Purpose: MEOWSTRO
//
//

#include "GameStateManager.hpp"
#include "ResourceManager.hpp"
#include "InputHandler.hpp"
#include "RenderWindow.hpp"
#include "GameConfig.hpp"
#include "AudioLogic.hpp"
#include "GameStats.hpp"
#include "Entity.hpp"
#include "Sprite.hpp"
#include "Audio.hpp"
#include "Font.hpp"

#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <random>

int main(int argc, char** argv)
{
	// Add tests later, Audio fails to find endpoint in Git Actions
	if (argc > 1 && std::string(argv[1]) == "--test") 
		return 0;
	if (SDL_Init(SDL_INIT_VIDEO) > 0)
		std::cout << "SDL_Init has failed, SDL ERROR: " << SDL_GetError();
	if (!(IMG_Init(IMG_INIT_PNG)))
		std::cout << "IMG_Init has failed, SDL ERROR: " << SDL_GetError();
	if (TTF_Init() == -1)
		std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;

	const auto& config = GameConfig::getInstance();
	const auto& windowConfig = config.getWindowConfig();
	RenderWindow window(windowConfig.title, windowConfig.width, windowConfig.height, windowConfig.flags);
	ResourceManager resourceManager(window.getRenderer());
	InputHandler inputHandler;

	srand(static_cast<unsigned int>(time(NULL)));
	
	// Create the game state manager and run the game
	GameStateManager gameStateManager(window, resourceManager, inputHandler);
	gameStateManager.run();

	TTF_Quit();
	SDL_Quit();

	return 0;
}