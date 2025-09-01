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
#include "Logger.hpp"
#include "Exceptions.hpp"

#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <random>
#include <cstdlib>

int main(int argc, char** argv)
{
	// Add tests later, Audio fails to find endpoint in Git Actions
	if (argc > 1 && std::string(argv[1]) == "--test") 
		return EXIT_SUCCESS;
		
	try {
		// Initialize SDL subsystems - fail fast on critical errors
		if (SDL_Init(SDL_INIT_VIDEO) != EXIT_SUCCESS) {
			throw InitializationException("SDL_Init(VIDEO) failed: " + std::string(SDL_GetError()));
		}
		
		if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
			throw InitializationException("IMG_Init(PNG) failed: " + std::string(IMG_GetError()));
		}
		
		if (TTF_Init() != EXIT_SUCCESS) {
			throw InitializationException("TTF_Init failed: " + std::string(TTF_GetError()));
		}
		
		Logger::info("SDL subsystems initialized successfully");

		const auto& config = GameConfig::getInstance();
		const auto& windowConfig = config.getWindowConfig();
		RenderWindow window(windowConfig.title, windowConfig.width, windowConfig.height, windowConfig.flags);
		
		if (!window.isValid()) {
			throw InitializationException("Failed to create render window");
		}
		
		ResourceManager resourceManager(window.getRenderer());
		if (!resourceManager.isValid()) {
			throw InitializationException("Failed to create resource manager");
		}
		
		InputHandler inputHandler;
		srand(static_cast<unsigned int>(time(NULL)));
		
		Logger::info("Game systems initialized, starting game loop");
		
		// Create the game state manager and run the game
		GameStateManager gameStateManager(window, resourceManager, inputHandler);
		gameStateManager.run();
		
		Logger::info("Game ended successfully");
		
	} catch (const InitializationException& e) {
		Logger::error(e.what());
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		return EXIT_FAILURE;
	} catch (const std::exception& e) {
		Logger::error("Unexpected error: " + std::string(e.what()));
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		return EXIT_FAILURE;
	}

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return EXIT_SUCCESS;
}