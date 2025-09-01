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

bool isTest = false;

void mainMenu(RenderWindow& window, ResourceManager& resourceManager, bool &gameRunning, SDL_Event &event, InputHandler& inputHandler);
void gameLoop(RenderWindow& window, ResourceManager& resourceManager, bool& gameRunning, SDL_Event& event, GameStats& stats, InputHandler& inputHandler);
void endScreen(RenderWindow& window, ResourceManager& resourceManager, bool& gameRunning, SDL_Event& event, GameStats& stats, InputHandler& inputHandler);

std::string formatScore(int score);

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

void mainMenu(RenderWindow &window, ResourceManager& resourceManager, bool &gameRunning, SDL_Event &event, InputHandler& inputHandler)
{
	const auto& config = GameConfig::getInstance();
	const auto& assetPaths = config.getAssetPaths();
	const auto& fontSizes = config.getFontSizes();
	const auto& visualConfig = config.getVisualConfig();
	
	bool onMenu = true;
	bool option = false; // false = start, true = quit
	
	SDL_Texture* quitTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.quitButton, "QUIT", visualConfig.YELLOW);
	SDL_Texture* startTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.menuButtons, "START", visualConfig.YELLOW);
	SDL_Texture* logoTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.menuLogo, "MEOWSTRO", visualConfig.YELLOW);
	SDL_Texture* logoCatTexture = resourceManager.loadTexture(assetPaths.menuCatTexture);
	SDL_Texture* selectedTexture = resourceManager.loadTexture(assetPaths.selectCatTexture);
	
	Entity quit(850, 800, quitTexture);
	Entity logo(715, 350, logoTexture);
	Entity start(850, 625, startTexture);
	Entity logoCat(660, 200, logoCatTexture);
	Sprite selectCat(760, 500, selectedTexture, 1, 1);

	while (onMenu)
	{
		while (SDL_PollEvent(&event))
		{
			InputAction action = inputHandler.processInput(event, GameState::MainMenu);
			
			switch (action) {
				case InputAction::Quit:
					onMenu = false;
					gameRunning = false;
					break;
					
				case InputAction::Select:
					onMenu = false;
					gameRunning = !option; // false = start game, true = quit
					break;
					
				case InputAction::MenuUp:
				case InputAction::MenuDown:
					option = !option; // Toggle between start/quit
					break;
					
				case InputAction::None:
				default:
					break;
			}
		}
		
		window.clear();
		// Position selector based on current option
		if (option)
			selectCat.setLoc(760, 775); // Quit position
		else
			selectCat.setLoc(760, 600); // Start position
		
		window.render(selectCat);
		window.render(logoCat);
		window.render(logo);
		window.render(start);
		window.render(quit);
		window.display();
	}
}

void endScreen(RenderWindow& window, ResourceManager& resourceManager, bool& gameRunning, SDL_Event& event, GameStats& stats, InputHandler& inputHandler)
{
	const auto& config = GameConfig::getInstance();
	const auto& assetPaths = config.getAssetPaths();
	const auto& fontSizes = config.getFontSizes();
	const auto& visualConfig = config.getVisualConfig();
	
	bool option = false;

	SDL_Texture* statsTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameStats, "GAME STATS", visualConfig.YELLOW);
	SDL_Texture* scoreTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, "SCORE", visualConfig.YELLOW);
	SDL_Texture* numberTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, formatScore(stats.getScore()), visualConfig.YELLOW);
	SDL_Texture* hitsTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, "HITS", visualConfig.YELLOW);
	SDL_Texture* numHitsTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, std::to_string(stats.getHits()), visualConfig.YELLOW);
	SDL_Texture* accuracyTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, "ACCURACY", visualConfig.YELLOW);
	SDL_Texture* accPercentTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, (std::to_string(stats.getAccuracy()) + "%"), visualConfig.YELLOW);
	SDL_Texture* missTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, "MISSES", visualConfig.YELLOW);
	SDL_Texture* numMissTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, std::to_string(stats.getMisses()), visualConfig.YELLOW);

	SDL_Texture* quitTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.quitButton, "QUIT", visualConfig.YELLOW);
	SDL_Texture* retryTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.quitButton, "RETRY", visualConfig.YELLOW);
	SDL_Texture* logoTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.menuLogo, "MEOWSTRO", visualConfig.YELLOW);
	SDL_Texture* selectedTexture = resourceManager.loadTexture(assetPaths.selectCatTexture);
	SDL_Texture* logoCatTexture = resourceManager.loadTexture(assetPaths.menuCatTexture);
	
	Entity titleStats(785, 325, statsTexture);
	Entity score(650, 400, scoreTexture);
	Entity number(1150, 400, numberTexture);
	Entity hits(650, 500, hitsTexture);
	Entity numHits(1150, 500, numHitsTexture);
	Entity accuracy(650, 600, accuracyTexture);
	Entity accPercent(1150, 600, accPercentTexture);
	Entity misses(650, 700, missTexture);
	Entity numMisses(1150, 700, numMissTexture);

	Entity quit(875, 900, quitTexture);
	Entity logo(735, 150, logoTexture);
	Entity retry(860, 800, retryTexture);
	Entity logoCat(675, 0, logoCatTexture);
	Sprite selectCat(775, 700, selectedTexture, 1, 1);

	while (!gameRunning)
	{
		while (SDL_PollEvent(&event))
		{
			InputAction action = inputHandler.processInput(event, GameState::EndScreen);
			
			switch (action) {
				case InputAction::Escape:
					return; // Return to main menu
					
				case InputAction::Select:
					gameRunning = !option; // false = retry, true = quit
					return;
					
				case InputAction::MenuUp:
				case InputAction::MenuDown:
					option = !option; // Toggle between retry/quit
					break;
					
				case InputAction::None:
				default:
					break;
			}
		}
		window.clear();
		// if represents selected option render location
		if (option)
			selectCat.setLoc(775, 900);
		else
			selectCat.setLoc(775, 775);

		window.render(selectCat);
		window.render(logoCat);
		window.render(logo);
		window.render(retry);
		window.render(quit);
		window.render(titleStats);
		window.render(score);
		window.render(number);
		window.render(hits);
		window.render(numHits);
		window.render(accuracy);
		window.render(accPercent);
		window.render(misses);
		window.render(numMisses);
		window.display();
	}
}

std::string formatScore(int score)
{
	std::ostringstream ss;
	ss << std::setw(6) << std::setfill('0') << score;
	return ss.str();
}