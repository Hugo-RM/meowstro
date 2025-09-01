#include "GameStateManager.hpp"
#include "RenderWindow.hpp"
#include "ResourceManager.hpp"
#include "GameConfig.hpp"

#include <iostream>

// Forward declarations of the original functions 
void mainMenu(RenderWindow& window, ResourceManager& resourceManager, bool& gameRunning, SDL_Event& event, InputHandler& inputHandler);
void endScreen(RenderWindow& window, ResourceManager& resourceManager, bool& gameRunning, SDL_Event& event, GameStats& stats, InputHandler& inputHandler);

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
    bool gameRunning = true;
    
    // Use the existing mainMenu function
    mainMenu(window, resourceManager, gameRunning, event, inputHandler);
    
    if (gameRunning) {
        // Player selected START
        resetGameStats();
        transitionTo(GameState::Playing);
    } else {
        // Player selected QUIT or pressed ESC
        transitionTo(GameState::Quit);
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
    bool gameRunning = false;
    
    endScreen(window, resourceManager, gameRunning, event, gameStats, inputHandler);
    
    if (gameRunning) {
        // Player selected RETRY
        resetGameStats();
        transitionTo(GameState::Playing);
    } else {
        transitionTo(GameState::Quit);
    }
}

void GameStateManager::resetGameStats()
{
    gameStats.resetStats();
}