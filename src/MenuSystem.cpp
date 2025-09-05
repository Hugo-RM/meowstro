#include "MenuSystem.hpp"
#include "GameConfig.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>

MenuSystem::MenuSystem() 
    : currentMenuType(MenuType::MainMenu)
    , currentOption(0)
    , menuActive(false)
{
}

MenuResult MenuSystem::runMainMenu(RenderWindow& window, ResourceManager& resourceManager, InputHandler& inputHandler) {
    resetMenuState(MenuType::MainMenu);
    
    const auto& config = GameConfig::getInstance();
    const auto& assetPaths = config.getAssetPaths();
    const auto& fontSizes = config.getFontSizes();
    const auto& visualConfig = config.getVisualConfig();
    
    // Create menu textures
    SDL_Texture* quitTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.quitButton, "QUIT", visualConfig.YELLOW);
    SDL_Texture* startTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.menuButtons, "START", visualConfig.YELLOW);
    SDL_Texture* logoTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.menuLogo, "MEOWSTRO", visualConfig.YELLOW);
    SDL_Texture* logoCatTexture = resourceManager.loadTexture(assetPaths.menuCatTexture);
    SDL_Texture* selectedTexture = resourceManager.loadTexture(assetPaths.selectCatTexture);
    
    // Create menu entities
    Entity quit(850, 800, quitTexture);
    Entity logo(715, 350, logoTexture);
    Entity start(850, 625, startTexture);
    Entity logoCat(660, 200, logoCatTexture);
    Sprite selectCat(760, 500, selectedTexture, 1, 1);
    
    SDL_Event event;
    
    while (menuActive) {
        while (SDL_PollEvent(&event)) {
            InputAction action = inputHandler.processInput(event, GameState::MainMenu);
            
            switch (action) {
                case InputAction::Quit:
                    return MenuResult::QuitGame;
                    
                case InputAction::Select:
                    // currentOption: 0 = start, 1 = quit
                    if (currentOption == 0) {
                        return MenuResult::StartGame;
                    } else {
                        return MenuResult::QuitGame;
                    }
                    
                case InputAction::MenuUp:
                case InputAction::MenuDown:
                    handleMenuNavigation(action, 2); // 2 options: start/quit
                    break;
                    
                case InputAction::None:
                default:
                    break;
            }
        }
        
        // Update selector position
        updateSelectorPosition(selectCat, MenuType::MainMenu);
        
        // Render menu
        window.clear();
        window.render(selectCat);
        window.render(logoCat);
        window.render(logo);
        window.render(start);
        window.render(quit);
        window.display();
    }
    
    return MenuResult::None;
}

MenuResult MenuSystem::runEndScreen(RenderWindow& window, ResourceManager& resourceManager, GameStats& stats, InputHandler& inputHandler) {
    resetMenuState(MenuType::EndScreen);
    
    const auto& config = GameConfig::getInstance();
    const auto& assetPaths = config.getAssetPaths();
    const auto& fontSizes = config.getFontSizes();
    const auto& visualConfig = config.getVisualConfig();
    
    // Create stats textures
    SDL_Texture* statsTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameStats, "GAME STATS", visualConfig.YELLOW);
    SDL_Texture* scoreTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, "SCORE", visualConfig.YELLOW);
    SDL_Texture* numberTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, formatScore(stats.getScore()), visualConfig.YELLOW);
    SDL_Texture* hitsTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, "HITS", visualConfig.YELLOW);
    SDL_Texture* numHitsTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, std::to_string(stats.getHits()), visualConfig.YELLOW);
    SDL_Texture* accuracyTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, "ACCURACY", visualConfig.YELLOW);
    SDL_Texture* accPercentTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, (std::to_string(stats.getAccuracy()) + "%"), visualConfig.YELLOW);
    SDL_Texture* missTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, "MISSES", visualConfig.YELLOW);
    SDL_Texture* numMissTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.gameScore, std::to_string(stats.getMisses()), visualConfig.YELLOW);
    
    // Create menu textures
    SDL_Texture* quitTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.quitButton, "QUIT", visualConfig.YELLOW);
    SDL_Texture* retryTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.quitButton, "RETRY", visualConfig.YELLOW);
    SDL_Texture* logoTexture = resourceManager.createTextTexture(assetPaths.fontPath, fontSizes.menuLogo, "MEOWSTRO", visualConfig.YELLOW);
    SDL_Texture* selectedTexture = resourceManager.loadTexture(assetPaths.selectCatTexture);
    SDL_Texture* logoCatTexture = resourceManager.loadTexture(assetPaths.menuCatTexture);
    
    // Create entities
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
    
    SDL_Event event;
    
    while (menuActive) {
        while (SDL_PollEvent(&event)) {
            InputAction action = inputHandler.processInput(event, GameState::EndScreen);
            
            switch (action) {
                case InputAction::Escape:
                    return MenuResult::GoToMainMenu;
                    
                case InputAction::Select:
                    // currentOption: 0 = retry, 1 = quit
                    if (currentOption == 0) {
                        return MenuResult::RetryGame;
                    } else {
                        return MenuResult::QuitGame;
                    }
                    
                case InputAction::MenuUp:
                case InputAction::MenuDown:
                    handleMenuNavigation(action, 2); // 2 options: retry/quit
                    break;
                    
                case InputAction::None:
                default:
                    break;
            }
        }
        
        // Update selector position
        updateSelectorPosition(selectCat, MenuType::EndScreen);
        
        // Render menu
        window.clear();
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
    
    return MenuResult::None;
}

// Future menu implementations
MenuResult MenuSystem::runPauseMenu(RenderWindow& window, ResourceManager& resourceManager, InputHandler& inputHandler) {
    // TODO: Implement pause menu for future expansion
    return MenuResult::None;
}

MenuResult MenuSystem::runSettingsMenu(RenderWindow& window, ResourceManager& resourceManager, InputHandler& inputHandler) {
    // TODO: Implement settings menu for future expansion
    return MenuResult::None;
}

void MenuSystem::resetMenuState(MenuType type) {
    currentMenuType = type;
    currentOption = 0;
    menuActive = true;
}

void MenuSystem::handleMenuNavigation(InputAction action, int maxOptions) {
    switch (action) {
        case InputAction::MenuUp:
            currentOption = (currentOption > 0) ? currentOption - 1 : maxOptions - 1;
            break;
        case InputAction::MenuDown:
            currentOption = (currentOption < maxOptions - 1) ? currentOption + 1 : 0;
            break;
        default:
            break;
    }
}

void MenuSystem::updateSelectorPosition(Sprite& selector, MenuType menuType) {
    switch (menuType) {
        case MenuType::MainMenu:
            if (currentOption == 0) {
                selector.setLoc(760, 600); // Start position
            } else {
                selector.setLoc(760, 775); // Quit position
            }
            break;
            
        case MenuType::EndScreen:
            if (currentOption == 0) {
                selector.setLoc(775, 775); // Retry position
            } else {
                selector.setLoc(775, 900); // Quit position
            }
            break;
            
        default:
            break;
    }
}

std::string MenuSystem::formatScore(int score) {
    std::ostringstream ss;
    ss << std::setw(6) << std::setfill('0') << score;
    return ss.str();
}