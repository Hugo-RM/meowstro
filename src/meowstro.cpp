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

void gameLoop(RenderWindow& window, ResourceManager& resourceManager, bool& gameRunning, SDL_Event& event, GameStats& stats, InputHandler& inputHandler)
{
	auto& config = GameConfig::getInstance();
	config.initializeBeatTimings(); // Initialize beat timings
	
	const auto& assetPaths = config.getAssetPaths();
	const auto& fontSizes = config.getFontSizes();
	const auto& visualConfig = config.getVisualConfig();
	const auto& audioConfig = config.getAudioConfig();
	const auto& gameplayConfig = config.getGameplayConfig();
	
	int fishStartX = 1920;

	// Textures loaded via ResourceManager
	SDL_Texture* fishTextures[3];
	fishTextures[0] = resourceManager.loadTexture(assetPaths.blueFishTexture);
	fishTextures[1] = resourceManager.loadTexture(assetPaths.greenFishTexture);
	fishTextures[2] = resourceManager.loadTexture(assetPaths.goldFishTexture);
	SDL_Texture* oceanTexture = resourceManager.loadTexture(assetPaths.oceanTexture);
	SDL_Texture* boatTexture = resourceManager.loadTexture(assetPaths.boatTexture);
	SDL_Texture* fisherTexture = resourceManager.loadTexture(assetPaths.fisherTexture);
	SDL_Texture* hookTexture = resourceManager.loadTexture(assetPaths.hookTexture);
	SDL_Texture* scoreTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, "SCORE", visualConfig.BLACK);
	SDL_Texture* numberTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameNumbers, "000000", visualConfig.BLACK);
	SDL_Texture* perfectHitTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.hitFeedback, "1000", visualConfig.RED);
	SDL_Texture* goodHitTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.hitFeedback, "500", visualConfig.RED);

	// Sprites & Background
	Entity ocean(0, 0, oceanTexture);
	Entity score(1720, 100, scoreTexture);
	Entity number(1720, 150, numberTexture);
	Sprite fisher(300, 200, fisherTexture, 1, 2);
	Sprite boat(150, 350, boatTexture, 1, 1);
	Sprite hook(430, 215, hookTexture, 1, 1);
	std::vector<Sprite> fish;
	fish.reserve(gameplayConfig.numBeats);
	std::unordered_set<int> fishHits; // index of fish hit
	std::unordered_map<int, Uint32> fishHitTimes; // index -> time of hit
	std::unordered_map<int, bool> fishHitTypes; // index -> false (Good), true (Perfect)

	for (int i = 0; i < gameplayConfig.numBeats; ++i)
	{
		fish.emplace_back(Sprite(gameplayConfig.fishStartXLocations[i], 720, fishTextures[rand() % gameplayConfig.numFishTextures], 1, 6));
	}

	float timeCounter = 0.0f;

	int songStartTime = SDL_GetTicks(); //Gets current ticks for better
	int throwDuration = gameplayConfig.throwDuration; // for hook sprite
	int hookTargetX = gameplayConfig.hookTargetX; // Fish location
	int hookTargetY = gameplayConfig.hookTargetY; 
	int fishTargetX = gameplayConfig.fishTargetX;
	int thrownTimer = 2; // for fisher sprite
	int hookStartX;
	int hookStartY;
	int sway = 0;
	int bob = 0;
	int handX; // Fisher's hand position for hook throwing
	int handY; 
	
	bool isReturning = false; // for hook sprite 
	bool isThrowing = false; // for hook sprite
	bool keydown = false; // Bool for the key
	bool thrown = false; // for fisher sprite
	
	Uint32 throwStartTime = 0;

	Audio player;
	AudioLogic gamePlay;
	player.playBackgroundMusic(audioConfig.backgroundMusicPath);

	std::vector<bool> noteHitFlags(gameplayConfig.numBeats * 2, false); //This bool checks for the continueity (if the note has passed) regardless of getting hit. Overall helping with syncing

	const std::vector<double>& noteBeats = gameplayConfig.noteBeats;

	while (gameRunning)
	{
		if (inputHandler.isKeyPressed(SDL_SCANCODE_SPACE)) //Checks the current state of the key and if true it makes the bool to be true (making it not work) unless not press down
			keydown = true;

		double currentTime = SDL_GetTicks() - songStartTime; //calculates the delay by comparing the current ticks and when the song starts

		// Only update score texture when score actually changes
		static int lastScore = -1;
		int currentScore = stats.getScore();
		if (currentScore != lastScore) {
			std::string strNum = formatScore(currentScore);
			numberTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameNumbers, strNum, visualConfig.BLACK);
			number.setTexture(numberTexture);
			lastScore = currentScore;
		}
		handX = fisher.getX() + 135;
		handY = fisher.getY() + 50;
		
		while (SDL_PollEvent(&event))
		{
			InputAction action = inputHandler.processInput(event, GameState::Playing);
			
			if (action == InputAction::Quit) {
				gameRunning = false;
				break;
			}
			else if (action == InputAction::Select && !keydown) { // Space key for rhythm timing
				if (!isThrowing)
				{
					thrown = true;
					thrownTimer = 2;
					isThrowing = true;
					isReturning = false;
					throwStartTime = SDL_GetTicks();
					hookStartX = handX;
					hookStartY = handY;
					hookTargetX = handX + 300;
					hookTargetY = handY + 475;
				}
				// int j = 0;
				for (int i = 0; i < noteBeats.size(); ++i) {
					// if (noteHitFlags[i])
					// {
					// 	j = 0;
					// 	continue;
					// }
					
					double expected = noteBeats[i];
					double delta = fabs(currentTime - expected); //Calculates the gurrent gap for the hit
					// if (j == 0)
					// 	std::cout << "Delta: " << delta << std::endl;
					// j++;
					if (delta <= gamePlay.getGOOD()) {
						short int scoreType = gamePlay.checkHit(expected, currentTime); //This compares the time the SPACE or DOWN was pressed to the time it is requires for the PERFECT or GOOD or Miss
						noteHitFlags[i] = true;
						fishHits.insert(i);
						fishHitTimes[i] = SDL_GetTicks(); // Record when hit occurred
						if (scoreType == 2) 
						{
							stats++;
							stats.increaseScore(1000);
							fishHitTypes[i] = true;
						}
						else if (scoreType == 1)
						{
							stats++;
							stats.increaseScore(500);
							fishHitTypes[i] = false;
						}
						break;
					}
				}
			}
		}
		for (int i = 0; i < noteBeats.size(); ++i) {
			if (noteHitFlags[i]) continue;

			double noteTime = noteBeats[i];
			if (currentTime > noteTime + gamePlay.getGOOD()) {
				// std::cout << std::endl << "miss" << std::endl << std::endl;
				stats--;
				noteHitFlags[i] = true;
			}
		}
		window.clear();
		window.render(ocean);
		timeCounter += 0.05;

		// sways around sprites
		for (int i = 0; i < gameplayConfig.numBeats; i++)
		{
			sway = static_cast<int>(sin(timeCounter + i) * 1.1);
			bob = static_cast<int>(cos(timeCounter + i) * 1.1);

			fish[i].setLoc(fish[i].getX() + sway, fish[i].getY() + bob);
		}

		hook.setLoc(hook.getX() + sway, hook.getY() + bob);
		boat.setLoc(boat.getX() + sway, boat.getY() + bob);
		fisher.setLoc(fisher.getX() + sway, fisher.getY() + bob);
		
		// Hand throwing sprite animation
		if (thrown)
		{
			fisher.setFrame(1, 2);
			thrownTimer--;

			if (thrownTimer <= 0)
			{
				thrown = false;
				fisher.setFrame(1, 1);
			}
		}
		else
		{
			fisher.setFrame(1, 1);
		}

		// Hook throwing animation
		if (isThrowing)
		{
			Uint32 now = SDL_GetTicks();
			Uint32 elapsed = now - throwStartTime;

			float progress = static_cast<float>(elapsed) / throwDuration;
			if (progress >= 1.0f)
			{
				progress = 1.0f;

				if (!isReturning)
				{
					isReturning = true;
					// makes start the new target
					std::swap(hookStartX, hookTargetX);
					std::swap(hookStartY, hookTargetY);
				}
				else
				{
					isThrowing = false;
					isReturning = false;
					hook.setLoc(hookStartX, hookStartY); // back to original location
				}
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

		Uint32 currentTicks = SDL_GetTicks();

		// render fish
		for (int i = 0; i < gameplayConfig.numBeats; i++)
		{
			if (fishHits.count(i)) 
			{
				// Fish was hit calculate time since hit
				Uint32 timeSinceHit = currentTicks - fishHitTimes[i];

				if (timeSinceHit < 1000) 
				{
					// Show text instead of fish for 1 second
					SDL_Texture* scoreTex = (fishHitTypes[i]) ? perfectHitTexture : goodHitTexture;

					SDL_Rect textRect;
					textRect.x = fish[i].getX(); // Same location as fish
					textRect.y = fish[i].getY() - 30; // Slightly above fish
					SDL_QueryTexture(scoreTex, NULL, NULL, &textRect.w, &textRect.h);

					SDL_RenderCopy(window.getRenderer(), scoreTex, NULL, &textRect);
				}

				continue; // Skip rendering the fish itself
			}

			// Move and render normal fish
			fish[i].moveLeft(15);
			window.render(fish[i]);
			fish[i]++;
			if (fish[i].getCol() == 4)
				fish[i].resetFrame(); // dead fish frames were 4 and on
		}

		window.render(boat);
		window.render(hook);
		window.render(fisher);
		window.render(score);
		window.render(number);

		window.display();
		keydown = false; // prevents holding space
		
		// Break the loop if music stopped playing
		if (Mix_PlayingMusic() == 0)
			gameRunning = false;

		SDL_Delay(visualConfig.frameDelay);
	}
	player.stopBackgroundMusic();
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