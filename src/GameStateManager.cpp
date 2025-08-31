#include "GameStateManager.hpp"
#include "RenderWindow.hpp"
#include "ResourceManager.hpp"
#include "GameConfig.hpp"
#include <iostream>

// Forward declarations of the original functions 
void mainMenu(RenderWindow& window, ResourceManager& resourceManager, bool& gameRunning, SDL_Event& event, InputHandler& inputHandler);
void gameLoop(RenderWindow& window, ResourceManager& resourceManager, bool& gameRunning, SDL_Event& event, GameStats& stats, InputHandler& inputHandler);
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
            runMainMenu();
            break;
            
        case GameState::Playing:
            runGameplay();
            break;
            
        case GameState::EndScreen:
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
    bool gameRunning = true;
    
    gameLoop(window, resourceManager, gameRunning, event, gameStats, inputHandler);
    
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