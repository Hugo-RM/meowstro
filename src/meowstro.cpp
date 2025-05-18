// Names: Hugo, Jaime, Jay, Leo
// Last Modified: 05/17/25
// Purpose: MEOWSTRO
//
//

#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <random>

#include "RenderWindow.hpp"
#include "AudioLogic.hpp"
#include "GameStats.hpp"
#include "Entity.hpp"
#include "Sprite.hpp"
#include "Audio.hpp"
#include "Font.hpp"

const char* comicSans = "../assets/fonts/Comic Sans MS.ttf";
const int NUM_OF_BEATS = 25;
const int NUM_FISH_TEXTURES = 3;
const SDL_Color YELLOW = { 255, 255, 100, 255 };
const int FISH_START_X_LOCS[NUM_OF_BEATS] = { 1352, 2350, 2465, 2800, 3145,
											  3330, 3480, 3663, 4175, 4560,
											  4816, 5245, 6059, 6260, 6644,
											  6885, 7100, 7545, 7801, 8230,
											  8775, 9145, 9531, 9829, 10160};

void mainMenu(RenderWindow& window, bool &gameRunning, SDL_Event &event);
void gameLoop(RenderWindow& window, bool& gameRunning, SDL_Event& event, GameStats& stats);
void endScreen(RenderWindow& window, bool& gameRunning, SDL_Event& event, GameStats& stats);

std::string formatScore(int score);

int main(int argc, char* args[])
{

	if (SDL_Init(SDL_INIT_VIDEO) > 0)
		std::cout << "SDL_Init has failed, SDL ERROR: " << SDL_GetError();
	if (!(IMG_Init(IMG_INIT_PNG)))
		std::cout << "IMG_Init has failed, SDL ERROR: " << SDL_GetError();
	if (TTF_Init() == -1)
		std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;

	RenderWindow window("Meowstro", 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

#ifdef CI_BUILD
	SDL_Delay(3000);
	SDL_Quit();
	return 0;
#endif

	srand(static_cast<unsigned int>(time(NULL)));
	bool gameRunning = true;
	GameStats stats;
	SDL_Event event;
	while (gameRunning)
	{
		mainMenu(window, gameRunning, event);
		if (gameRunning)
		{
			gameLoop(window, gameRunning, event, stats);
			std::cout << stats;
			endScreen(window, gameRunning, event, stats);
			stats.resetStats();
		}
	}
	window.~RenderWindow();

	TTF_Quit();
	SDL_Quit();

	return 0;
}

void mainMenu(RenderWindow &window, bool &gameRunning, SDL_Event &event)
{
	bool onMenu = true;
	bool option = false;
	Font logoFont, startFont, quitFont;
	logoFont.load(comicSans, 75);
	startFont.load(comicSans, 55);
	quitFont.load(comicSans, 65);
	SDL_Texture* quitTexture = quitFont.renderText(window.getRenderer(), "QUIT", YELLOW);
	SDL_Texture* startTexture = startFont.renderText(window.getRenderer(), "START", YELLOW);
	SDL_Texture* logoTexture = logoFont.renderText(window.getRenderer(), "MEOWSTRO", YELLOW);
	SDL_Texture* logoCatTexture = window.loadTexture("../assets/images/menu_cat.png");
	SDL_Texture* selectedTexture = window.loadTexture("../assets/images/select_cat.png");
	Entity quit(850, 800, quitTexture);
	Entity logo(715, 350, logoTexture);
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
					option = (option) ? false : true;
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

void gameLoop(RenderWindow& window, bool& gameRunning, SDL_Event& event, GameStats& stats)
{
	static int bpm = 147;
	//int locationsX[NUM_FISH_TEXTURES] = {  };
	int fishStartX = 1920;
	const int MAX_VISIBLE_NOTES = 12;

	Font scoreFont, numberFont, perfectHitFont, goodHitFont;
	scoreFont.load(comicSans, 40);
	numberFont.load(comicSans, 35);
	perfectHitFont.load(comicSans, 30);
	goodHitFont.load(comicSans, 30);

	// Textures
	SDL_Texture* fishTextures[NUM_FISH_TEXTURES];
	fishTextures[0] = window.loadTexture("../assets/images/blue_fish.png");
	fishTextures[1] = window.loadTexture("../assets/images/green_fish.png");
	fishTextures[2] = window.loadTexture("../assets/images/gold_fish.png");
	SDL_Texture* oceanTexture = window.loadTexture("../assets/images/Ocean.png");
	SDL_Texture* boatTexture = window.loadTexture("../assets/images/boat.png");
	SDL_Texture* fisherTexture = window.loadTexture("../assets/images/fisher.png");
	SDL_Texture* hookTexture = window.loadTexture("../assets/images/hook.png");
	SDL_Texture* scoreTexture = scoreFont.renderText(window.getRenderer(), "SCORE", { 0, 0, 0, 255 });
	SDL_Texture* numberTexture = numberFont.renderText(window.getRenderer(), "000000", { 0, 0, 0, 255 });
	SDL_Texture* perfectHitTexture = perfectHitFont.renderText(window.getRenderer(), "1000", { 255, 0, 0, 255 });
	SDL_Texture* goodHitTexture = goodHitFont.renderText(window.getRenderer(), "500", { 255, 0, 0, 255 });

	// Sprites & Background
	Entity ocean(0, 0, oceanTexture);
	Entity score(1720, 100, scoreTexture);
	Entity number(1720, 150, numberTexture);
	Sprite fisher(300, 200, fisherTexture, 1, 2);
	Sprite boat(150, 350, boatTexture, 1, 1);
	Sprite hook(430, 215, hookTexture, 1, 1);
	std::vector<Sprite> fish; 
	std::unordered_set<int> fishHits; // index of fish hit
	std::unordered_map<int, Uint32> fishHitTimes; // index -> time of hit
	std::unordered_map<int, bool> fishHitTypes; // index -> false (Good), true (Perfect)
	
	for (int i = 0; i < 25; i++)
	{
		fish.push_back(Sprite(FISH_START_X_LOCS[i], 720, fishTextures[rand() % 3], 1, 6));
	}

	float timeCounter = 0.0f;

	const double travelDuration = 2000.0; // ms before beat to start moving
	int songStartTime = SDL_GetTicks(); //Gets current ticks for better
	int throwDuration = 200; // for hook sprite
	int hookTargetX = 650; // Fish location
	int hookTargetY = 625; 
	int fishTargetX = 660;
	int thrownTimer = 2; // for fisher sprite
	int hookStartX;
	int hookStartY;
	int sway = 0;
	int bob = 0;
	int handX; // This cat is thor but with a spear hook thing
	int handY; 

	
	bool isReturning = false; // for hook sprite 
	bool isThrowing = false; // for hook sprite
	bool keydown = false; //Bool for the key
	bool thrown = false; // for fisher sprite
	

	Uint32 throwStartTime = 0;


	Audio player;
	AudioLogic gamePlay;
	player.playBackgroundMusic("../assets/audio/meowstro_short_ver.mp3");

	std::vector<bool> noteHitFlags(49, false); //This bool checks for the continueity (if the note has passed) regardless of getting hit. Overall helping with syncing

	std::vector<double> noteBeats = { //The vector contains the clicks in miliseconds
	gamePlay.msFromMscs(0,3,46), gamePlay.msFromMscs(0,7,75), gamePlay.msFromMscs(0,9,38), gamePlay.msFromMscs(0,10,61), gamePlay.msFromMscs(0,12,24), gamePlay.msFromMscs(0,13,06),
	gamePlay.msFromMscs(0,13,87), gamePlay.msFromMscs(0,15,30), gamePlay.msFromMscs(0,17,95), gamePlay.msFromMscs(0,20,0), gamePlay.msFromMscs(0,21,22), gamePlay.msFromMscs(0,23,26),
	gamePlay.msFromMscs(0,27,14), gamePlay.msFromMscs(0,28,57), gamePlay.msFromMscs(0,30,40), gamePlay.msFromMscs(0,31,93), gamePlay.msFromMscs(0,32,65), gamePlay.msFromMscs(0,34,69),
	gamePlay.msFromMscs(0,35,91), gamePlay.msFromMscs(0,37,95), gamePlay.msFromMscs(0,41,83), gamePlay.msFromMscs(0,43,26), gamePlay.msFromMscs(0,45,10), gamePlay.msFromMscs(0,46,52),
	gamePlay.msFromMscs(0,48,57)
	};

	const Uint8* state = SDL_GetKeyboardState(NULL);

	while (gameRunning)
	{
		if (state[SDL_SCANCODE_SPACE]) //Checks the current state of the key and if true it makes the bool to be true (making it not work) unless not press down
			keydown = true;

		double currentTime = SDL_GetTicks() - songStartTime; //calculates the delay by comparing the current ticks and when the song starts

		std::string strNum = formatScore(stats.getScore());
		numberTexture = numberFont.renderText(window.getRenderer(), strNum, { 0, 0, 0, 255 });
		number.setTexture(numberTexture);
		handX = fisher.getX() + 135;
		handY = fisher.getY() + 50;
		
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) { //Exit key, stop the geames
				gameRunning = false;
				break;
			}
			else if (!keydown && event.key.keysym.sym == SDLK_SPACE) { //Space and down arrow are use to hit or make the clicks
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
				int j = 0;
				for (int i = 0; i < noteBeats.size(); ++i) {
					if (noteHitFlags[i])
					{
						j = 0;
						continue;
					}
					
					double expected = noteBeats[i];
					double delta = fabs(currentTime - expected); //Calculates the gurrent gap for the hit
					if (j == 0)
						std::cout << "Delta: " << delta << std::endl;
					j++;
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
				std::cout << std::endl << "miss" << std::endl << std::endl;
				stats--;
				noteHitFlags[i] = true;
			}
		}
		window.clear();
		window.render(ocean);
		timeCounter += 0.05;

		// sways around sprites
		for (int i = 0; i < NUM_OF_BEATS; i++)
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
		for (int i = 0; i < NUM_OF_BEATS; i++)
		{
			if (fishHits.count(i)) 
			{
				// Fish was hit � calculate time since hit
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
			window.render(fish[i]++);
			if (fish[i].getCol() == 4)
				fish[i].resetFrame(); // dead fish frames past 4�6
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
		{
			gameRunning = false;
			std::cout << "Music ended � exiting game loop." << std::endl;
		}

		SDL_Delay(75);
	}
	player.stopBackgroundMusic();
	player.~Audio();
	scoreFont.unload();
	numberFont.unload();
}

void endScreen(RenderWindow& window, bool& gameRunning, SDL_Event& event, GameStats& stats)
{
	bool option = false;
	Font logoFont, retryFont, quitFont,
		statsFont, scoreFont, numberFont, 
		hitsFont, numHitsFont, accuracyFont,
		accPercentFont, missFont, numMissFont;

	logoFont.load(comicSans, 75);
	retryFont.load(comicSans, 65);
	quitFont.load(comicSans, 65);
	
	statsFont.load(comicSans, 55);
	scoreFont.load(comicSans, 40);
	numberFont.load(comicSans, 40);
	hitsFont.load(comicSans, 40);
	numHitsFont.load(comicSans, 40);
	accuracyFont.load(comicSans, 40);
	accPercentFont.load(comicSans, 40);
	missFont.load(comicSans, 40);
	numMissFont.load(comicSans, 40);

	SDL_Texture* statsTexture = statsFont.renderText(window.getRenderer(), "GAME STATS", YELLOW);
	SDL_Texture* scoreTexture = scoreFont.renderText(window.getRenderer(), "SCORE", YELLOW);
	SDL_Texture* numberTexture = numberFont.renderText(window.getRenderer(), formatScore(stats.getScore()), YELLOW);
	SDL_Texture* hitsTexture = hitsFont.renderText(window.getRenderer(), "HITS", YELLOW);
	SDL_Texture* numHitsTexture = numHitsFont.renderText(window.getRenderer(), std::to_string(stats.getHits()), YELLOW);
	SDL_Texture* accuracyTexture = accuracyFont.renderText(window.getRenderer(), "ACCURACY", YELLOW);
	SDL_Texture* accPercentTexture = accPercentFont.renderText(window.getRenderer(), (std::to_string(stats.getAccuracy()) + "%"), YELLOW);
	SDL_Texture* missTexture = missFont.renderText(window.getRenderer(), "MISSES", YELLOW);
	SDL_Texture* numMissTexture = numMissFont.renderText(window.getRenderer(), std::to_string(stats.getMisses()), YELLOW);

	SDL_Texture* quitTexture = quitFont.renderText(window.getRenderer(), "QUIT", YELLOW);
	SDL_Texture* retryTexture = retryFont.renderText(window.getRenderer(), "RETRY", YELLOW);
	SDL_Texture* logoTexture = logoFont.renderText(window.getRenderer(), "MEOWSTRO", YELLOW);
	SDL_Texture* selectedTexture = window.loadTexture("../assets/images/select_cat.png");
	SDL_Texture* logoCatTexture = window.loadTexture("../assets/images/menu_cat.png");
	
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
			switch (event.type)
			{
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
				{
					return;
				}
				case SDLK_SPACE:
				{
					gameRunning = (option) ? false : true; // op0 = retry op1 = exit
					return;
				}
				case SDLK_UP:
				case SDLK_DOWN:
				{
					option = (option) ? false : true;
					break;
				}
				}
			}
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