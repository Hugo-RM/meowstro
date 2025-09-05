#pragma once

#include "RenderWindow.hpp"
#include "ResourceManager.hpp"
#include "InputHandler.hpp"
#include "GameStats.hpp"
#include "Entity.hpp"
#include "Sprite.hpp"

#include <SDL.h>

// Menu result types for different menu outcomes
enum class MenuResult {
    None,           // Still in menu
    StartGame,      // Start new game
    RetryGame,      // Retry current game
    QuitGame,       // Quit to desktop
    GoToMainMenu    // Return to main menu
};

// Menu types for future extensibility
enum class MenuType {
    MainMenu,
    EndScreen,
    PauseMenu,      // Future menu
    SettingsMenu,   // Future menu
    CreditsMenu     // Future menu
};

class MenuSystem {
public:
    MenuSystem();
    ~MenuSystem() = default;
    
    // Main menu interface
    MenuResult runMainMenu(RenderWindow& window, ResourceManager& resourceManager, InputHandler& inputHandler);
    
    // End screen interface
    MenuResult runEndScreen(RenderWindow& window, ResourceManager& resourceManager, GameStats& stats, InputHandler& inputHandler);
    
    // Future extensibility methods
    MenuResult runPauseMenu(RenderWindow& window, ResourceManager& resourceManager, InputHandler& inputHandler);
    MenuResult runSettingsMenu(RenderWindow& window, ResourceManager& resourceManager, InputHandler& inputHandler);
    
private:
    // Current menu state
    MenuType currentMenuType;
    int currentOption;
    bool menuActive;
    
    // Helper methods for menu management
    void resetMenuState(MenuType type);
    void handleMenuNavigation(InputAction action, int maxOptions);
    
    // Rendering helpers
    void renderMainMenuContent(RenderWindow& window, ResourceManager& resourceManager);
    void renderEndScreenContent(RenderWindow& window, ResourceManager& resourceManager, GameStats& stats);
    
    // Menu option management
    void updateSelectorPosition(Sprite& selector, MenuType menuType);
    
    // Utility for score formatting (moved from meowstro.cpp)
    std::string formatScore(int score);
};