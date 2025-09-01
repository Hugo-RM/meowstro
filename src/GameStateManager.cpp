#include "GameStateManager.hpp"
#include "RenderWindow.hpp"
#include "ResourceManager.hpp"
#include "GameConfig.hpp"

#include <iostream>

GameStateManager::GameStateManager(RenderWindow& window, ResourceManager& resourceManager, InputHandler& inputHandler)
    : currentState(GameState::MainMenu)
    , nextState(GameState::MainMenu)
    , window(window)
    , resourceManager(resourceManager)
    , inputHandler(inputHandler)
{
}

void GameStateManager::run()
{
    while (isRunning()) {
        updateState();
    }
}

void GameStateManager::transitionTo(GameState newState)
{
    nextState = newState;
}

void GameStateManager::updateState()
{
    // Handle state transitions
    if (currentState != nextState) {
        currentState = nextState;
    }
    
    // Execute current state
    switch (currentState) {
        case GameState::MainMenu:
            std::cout << "Entering Main Menu" << std::endl;
            runMainMenu();
            break;
            
        case GameState::Playing:
            std::cout << "Entering Gameplay" << std::endl;
            runGameplay();
            break;
            
        case GameState::EndScreen:
            std::cout << "Entering End Screen" << std::endl;
            runEndScreen();
            break;
            
        case GameState::Quit:
            // Will exit main loop
            break;
    }
}

void GameStateManager::runMainMenu()
{
    MenuResult result = menuSystem.runMainMenu(window, resourceManager, inputHandler);
    
    switch (result) {
        case MenuResult::StartGame:
            resetGameStats();
            transitionTo(GameState::Playing);
            break;
        case MenuResult::QuitGame:
            transitionTo(GameState::Quit);
            break;
        default:
            break;
    }
}

void GameStateManager::runGameplay()
{
    // Initialize the rhythm game
    rhythmGame.initialize(window, resourceManager, gameStats);
    bool exitEarly = false;
    // Main gameplay loop
    while (currentState == GameState::Playing && isRunning()) {
        exitEarly = false;
        
        // Process all SDL events this frame
        while (SDL_PollEvent(&event)) {
            InputAction action = inputHandler.processInput(event, GameState::Playing);
            
            // Process each action immediately instead of only keeping the last one
            if (action != InputAction::None) {
                if (!rhythmGame.update(action, inputHandler)) {
                    // Game ended (music finished or quit)
                    exitEarly = true;
                    break;
                }
            }
        }
        
        // Update game logic even when no input events occurred
        if (!exitEarly && !rhythmGame.update(InputAction::None, inputHandler)) {
            exitEarly = true;
        }
        
        // Render the game
        rhythmGame.render(window);
        
        // Check if we should exit the gameplay state
        if (rhythmGame.isGameOver(exitEarly)) {
            break;
        }
    }
    
    // Clean up rhythm game resources (stop music, etc.)
    rhythmGame.cleanup();
    
    std::cout << gameStats;
    
    transitionTo(GameState::EndScreen);
}

void GameStateManager::runEndScreen()
{
    MenuResult result = menuSystem.runEndScreen(window, resourceManager, gameStats, inputHandler);
    
    switch (result) {
        case MenuResult::RetryGame:
            resetGameStats();
            transitionTo(GameState::Playing);
            break;
        case MenuResult::GoToMainMenu:
            transitionTo(GameState::MainMenu);
            break;
        case MenuResult::QuitGame:
            transitionTo(GameState::Quit);
            break;
        default:
            break;
    }
}

void GameStateManager::resetGameStats()
{
    gameStats.resetStats();
}