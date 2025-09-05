#pragma once

#include "InputHandler.hpp"
#include "GameStats.hpp"
#include "RhythmGame.hpp"
#include "MenuSystem.hpp"

#include <SDL.h>

// Forward declarations
class RenderWindow;
class ResourceManager;

class GameStateManager
{
public:
    GameStateManager(RenderWindow& window, ResourceManager& resourceManager, InputHandler& inputHandler);
    ~GameStateManager() = default;
    
    // Main game loop - runs until quit
    void run();
    
    // Get current state
    GameState getCurrentState() const { return currentState; }
    
    // Check if game should continue running
    bool isRunning() const { return currentState != GameState::Quit; }
    
private:
    // State management
    GameState currentState;
    GameState nextState;
    
    // Game systems
    RenderWindow& window;
    ResourceManager& resourceManager; 
    InputHandler& inputHandler;
    GameStats gameStats;
    SDL_Event event;
    RhythmGame rhythmGame;
    MenuSystem menuSystem;
    
    // State transition methods
    void transitionTo(GameState newState);
    void updateState();
    
    // State execution methods
    void runMainMenu();
    void runGameplay();
    void runEndScreen();
    
    // Helper methods
    void resetGameStats();
};