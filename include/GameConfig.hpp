#pragma once

#include <SDL.h>
#include <string>
#include <vector>

class GameConfig
{
public:
    // Singleton pattern for global configuration access
    static GameConfig& getInstance();
    
    // Game window settings
    struct WindowConfig {
        const char *title = "Meowstro";
        int width = 1920;
        int height = 1080;
        Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    };
    
    // Audio and timing settings
    struct AudioConfig {
        int bpm = 147;
        double travelDuration = 2000.0; // ms before beat to start moving
        std::string backgroundMusicPath = "./assets/audio/meowstro_short_ver.mp3";
    };
    
    // Visual settings
    struct VisualConfig {
        const SDL_Color YELLOW = { 255, 255, 100, 255 };
        const SDL_Color BLACK = { 0, 0, 0, 255 };
        const SDL_Color RED = { 255, 0, 0, 255 };
        int frameDelay = 75; // SDL_Delay value
    };
    
    // Asset paths
    struct AssetPaths {
        std::string fontPath = "./assets/fonts/Comic Sans MS.ttf";
        
        // Image paths
        std::string oceanTexture = "./assets/images/Ocean.png";
        std::string boatTexture = "./assets/images/boat.png";
        std::string fisherTexture = "./assets/images/fisher.png";
        std::string hookTexture = "./assets/images/hook.png";
        std::string menuCatTexture = "./assets/images/menu_cat.png";
        std::string selectCatTexture = "./assets/images/select_cat.png";
        
        // Fish textures
        std::string blueFishTexture = "./assets/images/blue_fish.png";
        std::string greenFishTexture = "./assets/images/green_fish.png";
        std::string goldFishTexture = "./assets/images/gold_fish.png";
    };
    
    // Game mechanics
    struct GameplayConfig {
        int numBeats = 25;
        int numFishTextures = 3;
        int throwDuration = 200; // hook animation duration
        int hookTargetX = 650;
        int hookTargetY = 625;
        int fishTargetX = 660;
        
        // Fish spawn locations
        std::vector<int> fishStartXLocations = { 
            1352, 2350, 2465, 2800, 3145, 3330, 3480, 3663, 4175, 4560,
            4816, 5245, 6059, 6260, 6644, 6885, 7100, 7545, 7801, 8230,
            8775, 9145, 9531, 9829, 10160 
        };
        
        // Beat timing data - will be initialized from AudioLogic conversion
        std::vector<double> noteBeats;
    };
    
    // Font sizes
    struct FontSizes {
        int menuLogo = 75;
        int menuButtons = 55;
        int quitButton = 65;
        int gameScore = 40;
        int gameNumbers = 35;
        int gameStats = 55;
        int hitFeedback = 30;
    };
    
    // Initialization method for beat timings
    void initializeBeatTimings();
    
    // Getter methods
    const WindowConfig& getWindowConfig() const { return windowConfig; }
    const AudioConfig& getAudioConfig() const { return audioConfig; }
    const VisualConfig& getVisualConfig() const { return visualConfig; }
    const AssetPaths& getAssetPaths() const { return assetPaths; }
    const GameplayConfig& getGameplayConfig() const { return gameplayConfig; }
    const FontSizes& getFontSizes() const { return fontSizes; }
    
private:
    GameConfig() = default;
    ~GameConfig() = default;
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;
    
    WindowConfig windowConfig;
    AudioConfig audioConfig;
    VisualConfig visualConfig;
    AssetPaths assetPaths;
    GameplayConfig gameplayConfig;
    FontSizes fontSizes;
};