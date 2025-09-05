# Old Game Loop Code

This file will contain the entirity of a working gameLoop function from the main until finished refactoring to a fully functioning alternative that I am comfortable with. I could just use git commands but I'd like to have this here.

```cpp
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
```