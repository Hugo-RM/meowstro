#include <gtest/gtest.h>
#include "ResourceManager.hpp"
#include "GameConfig.hpp"
#include "RenderWindow.hpp"
#include "Logger.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <fstream>

class AssetLoadingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize SDL subsystems for testing
        ASSERT_EQ(SDL_Init(SDL_INIT_VIDEO), 0) << "SDL_Init failed: " << SDL_GetError();
        ASSERT_NE(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG, 0) << "IMG_Init failed: " << IMG_GetError();
        ASSERT_EQ(TTF_Init(), 0) << "TTF_Init failed: " << TTF_GetError();
        
        // Create a minimal render window for ResourceManager
        window = std::make_unique<RenderWindow>("Asset Test", 800, 600, SDL_WINDOW_HIDDEN);
        ASSERT_TRUE(window->isValid()) << "Failed to create test render window";
        
        // Create ResourceManager
        resourceManager = std::make_unique<ResourceManager>(window->getRenderer());
        ASSERT_TRUE(resourceManager->isValid()) << "Failed to create ResourceManager";
        
        // Check if assets are available in current directory or build directory
        checkAssetAvailability();
    }
    
private:
    void checkAssetAvailability() {
        // Test if assets are in the current directory (project root)
        std::ifstream testFile("./assets/images/Ocean.png");
        if (testFile.good()) {
            assetsAvailable = true;
            testFile.close();
            return;
        }
        
        // Test if we're running from build directory and assets are in parent
        std::ifstream testFileBuild("../assets/images/Ocean.png");
        if (testFileBuild.good()) {
            assetsAvailable = true;
            testFileBuild.close();
            // Update asset paths to point to parent directory
            updateAssetPaths("../assets/");
            return;
        }
        
        // Assets not found in either location
        assetsAvailable = false;
    }
    
    void updateAssetPaths(const std::string& basePath) {
        // This is a bit of a hack, but we need to temporarily modify GameConfig
        // In a real scenario, GameConfig should support different base paths
        alternativeBasePath = basePath;
    }

protected:
    std::string getAssetPath(const std::string& originalPath) const {
        if (!alternativeBasePath.empty() && originalPath.substr(0, 9) == "./assets/") {
            return alternativeBasePath + originalPath.substr(9); // Remove "./assets/" and add new base
        }
        return originalPath;
    }
    
    bool assetsAvailable = true;
    std::string alternativeBasePath;
    
    void TearDown() override {
        resourceManager.reset();
        window.reset();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
    
    std::unique_ptr<RenderWindow> window;
    std::unique_ptr<ResourceManager> resourceManager;
};

TEST_F(AssetLoadingTest, LoadAllImageAssets) {
    if (!assetsAvailable) {
        GTEST_SKIP() << "Assets not available in current directory - test requires game assets";
    }
    
    const auto& assetPaths = GameConfig::getInstance().getAssetPaths();
    
    // Test all image assets from GameConfig
    std::vector<std::pair<std::string, std::string>> imageAssets = {
        {"Ocean", getAssetPath(assetPaths.oceanTexture)},
        {"Boat", getAssetPath(assetPaths.boatTexture)},
        {"Fisher", getAssetPath(assetPaths.fisherTexture)},
        {"Hook", getAssetPath(assetPaths.hookTexture)},
        {"Menu Cat", getAssetPath(assetPaths.menuCatTexture)},
        {"Select Cat", getAssetPath(assetPaths.selectCatTexture)},
        {"Blue Fish", getAssetPath(assetPaths.blueFishTexture)},
        {"Green Fish", getAssetPath(assetPaths.greenFishTexture)},
        {"Gold Fish", getAssetPath(assetPaths.goldFishTexture)}
    };
    
    for (const auto& [name, path] : imageAssets) {
        SCOPED_TRACE("Loading image asset: " + name + " from " + path);
        
        SDL_Texture* texture = resourceManager->loadTexture(path);
        ASSERT_NE(texture, nullptr) << "Failed to load " << name << " from " << path;
        
        // Verify texture properties
        int w, h;
        Uint32 format;
        int access;
        int result = SDL_QueryTexture(texture, &format, &access, &w, &h);
        ASSERT_EQ(result, 0) << "Failed to query texture properties for " << name << ": " << SDL_GetError();
        EXPECT_GT(w, 0) << name << " has invalid width";
        EXPECT_GT(h, 0) << name << " has invalid height";
        
        // Test that texture is properly cached
        SDL_Texture* cachedTexture = resourceManager->loadTexture(path);
        EXPECT_EQ(texture, cachedTexture) << name << " should be cached and return same pointer";
    }
}

TEST_F(AssetLoadingTest, LoadFontAsset) {
    if (!assetsAvailable) {
        GTEST_SKIP() << "Assets not available in current directory - test requires game assets";
    }
    
    const auto& assetPaths = GameConfig::getInstance().getAssetPaths();
    const auto& fontSizes = GameConfig::getInstance().getFontSizes();
    std::string fontPath = getAssetPath(assetPaths.fontPath);
    
    // Test font loading with different sizes used in the game
    std::vector<std::pair<std::string, int>> fontTests = {
        {"Menu Logo", fontSizes.menuLogo},
        {"Menu Buttons", fontSizes.menuButtons},
        {"Quit Button", fontSizes.quitButton},
        {"Game Score", fontSizes.gameScore},
        {"Game Numbers", fontSizes.gameNumbers},
        {"Game Stats", fontSizes.gameStats},
        {"Hit Feedback", fontSizes.hitFeedback}
    };
    
    for (const auto& [name, size] : fontTests) {
        SCOPED_TRACE("Loading font: " + name + " size " + std::to_string(size));
        
        Font* font = resourceManager->getFont(fontPath, size);
        ASSERT_NE(font, nullptr) << "Failed to load font " << name << " at size " << size;
        
        // Test that font can create text textures
        SDL_Color testColor = {255, 255, 255, 255}; // White
        SDL_Texture* textTexture = resourceManager->createTextTexture(
            fontPath, size, "TEST", testColor
        );
        ASSERT_NE(textTexture, nullptr) << "Failed to create text texture with " << name << " font";
        
        // Verify text texture properties
        int w, h;
        Uint32 format;
        int access;
        int result = SDL_QueryTexture(textTexture, &format, &access, &w, &h);
        ASSERT_EQ(result, 0) << "Failed to query text texture properties: " << SDL_GetError();
        EXPECT_GT(w, 0) << "Text texture has invalid width";
        EXPECT_GT(h, 0) << "Text texture has invalid height";
    }
}

TEST_F(AssetLoadingTest, LoadGameUITextTextures) {
    if (!assetsAvailable) {
        GTEST_SKIP() << "Assets not available in current directory - test requires game assets";
    }
    
    const auto& assetPaths = GameConfig::getInstance().getAssetPaths();
    const auto& fontSizes = GameConfig::getInstance().getFontSizes();
    std::string fontPath = getAssetPath(assetPaths.fontPath);
    const auto& visualConfig = GameConfig::getInstance().getVisualConfig();
    
    // Test all UI text elements that appear in the game
    std::vector<std::pair<std::string, int>> uiTexts = {
        {"MEOWSTRO", fontSizes.menuLogo},
        {"START", fontSizes.menuButtons},
        {"QUIT", fontSizes.quitButton},
        {"RETRY", fontSizes.menuButtons},
        {"SCORE", fontSizes.gameScore},
        {"GAME STATS", fontSizes.gameStats},
        {"HITS", fontSizes.gameStats},
        {"MISSES", fontSizes.gameStats},
        {"ACCURACY", fontSizes.gameStats},
        {"000000", fontSizes.gameNumbers},  // Score format
        {"001000", fontSizes.gameNumbers},  // Score example
        {"100.000000%", fontSizes.gameNumbers}, // Accuracy format
        {"1000", fontSizes.hitFeedback},    // Perfect hit feedback
        {"500", fontSizes.hitFeedback}      // Good hit feedback
    };
    
    for (const auto& [text, fontSize] : uiTexts) {
        SCOPED_TRACE("Creating UI text: '" + text + "' at size " + std::to_string(fontSize));
        
        // Test with different colors used in the game
        std::vector<std::pair<std::string, SDL_Color>> colorTests = {
            {"Yellow", visualConfig.YELLOW},
            {"Black", visualConfig.BLACK},
            {"Red", visualConfig.RED}
        };
        
        for (const auto& [colorName, color] : colorTests) {
            SDL_Texture* textTexture = resourceManager->createTextTexture(
                fontPath, fontSize, text, color
            );
            ASSERT_NE(textTexture, nullptr) << "Failed to create " << colorName << " text: '" << text << "'";
            
            // Verify texture properties
            int w, h;
            Uint32 format;
            int access;
            int result = SDL_QueryTexture(textTexture, &format, &access, &w, &h);
            ASSERT_EQ(result, 0) << "Failed to query text texture: " << SDL_GetError();
            EXPECT_GT(w, 0) << "Text '" << text << "' has invalid width";
            EXPECT_GT(h, 0) << "Text '" << text << "' has invalid height";
        }
    }
}

TEST_F(AssetLoadingTest, TestResourceManagerCaching) {
    if (!assetsAvailable) {
        GTEST_SKIP() << "Assets not available in current directory - test requires game assets";
    }
    
    const auto& assetPaths = GameConfig::getInstance().getAssetPaths();
    std::string oceanPath = getAssetPath(assetPaths.oceanTexture);
    std::string fontPath = getAssetPath(assetPaths.fontPath);
    
    // Test that assets are properly cached
    SDL_Texture* texture1 = resourceManager->loadTexture(oceanPath);
    SDL_Texture* texture2 = resourceManager->loadTexture(oceanPath);
    EXPECT_EQ(texture1, texture2) << "Ocean texture should be cached";
    
    // Test text texture caching
    SDL_Color white = {255, 255, 255, 255};
    SDL_Texture* text1 = resourceManager->createTextTexture(fontPath, 30, "TEST", white);
    SDL_Texture* text2 = resourceManager->createTextTexture(fontPath, 30, "TEST", white);
    EXPECT_EQ(text1, text2) << "Text texture should be cached";
    
    // Test that different parameters create different textures
    SDL_Color red = {255, 0, 0, 255};
    SDL_Texture* text3 = resourceManager->createTextTexture(fontPath, 30, "TEST", red);
    EXPECT_NE(text1, text3) << "Different colored text should create different textures";
    
    SDL_Texture* text4 = resourceManager->createTextTexture(fontPath, 40, "TEST", white);
    EXPECT_NE(text1, text4) << "Different sized text should create different textures";
    
    SDL_Texture* text5 = resourceManager->createTextTexture(fontPath, 30, "DIFFERENT", white);
    EXPECT_NE(text1, text5) << "Different text content should create different textures";
}

TEST_F(AssetLoadingTest, TestResourceManagerValidation) {
    const auto& assetPaths = GameConfig::getInstance().getAssetPaths();
    
    // Test that ResourceManager properly handles invalid inputs
    SDL_Texture* nullTexture = resourceManager->loadTexture("");
    EXPECT_EQ(nullTexture, nullptr) << "Empty path should return nullptr";
    
    SDL_Texture* missingTexture = resourceManager->loadTexture("./assets/images/nonexistent.png");
    EXPECT_EQ(missingTexture, nullptr) << "Non-existent file should return nullptr";
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Texture* emptyTextTexture = resourceManager->createTextTexture(assetPaths.fontPath, 30, "", white);
    EXPECT_EQ(emptyTextTexture, nullptr) << "Empty text should return nullptr";
    
    SDL_Texture* invalidFontTexture = resourceManager->createTextTexture("nonexistent.ttf", 30, "TEST", white);
    EXPECT_EQ(invalidFontTexture, nullptr) << "Invalid font path should return nullptr";
}

// Integration test that mimics actual game asset loading patterns
TEST_F(AssetLoadingTest, GameSessionAssetLoadingPattern) {
    if (!assetsAvailable) {
        GTEST_SKIP() << "Assets not available in current directory - test requires game assets";
    }
    
    const auto& assetPaths = GameConfig::getInstance().getAssetPaths();
    const auto& fontSizes = GameConfig::getInstance().getFontSizes();
    std::string fontPath = getAssetPath(assetPaths.fontPath);
    const auto& visualConfig = GameConfig::getInstance().getVisualConfig();
    
    // Simulate main menu asset loading
    std::vector<SDL_Texture*> menuAssets = {
        resourceManager->createTextTexture(fontPath, fontSizes.menuLogo, "MEOWSTRO", visualConfig.YELLOW),
        resourceManager->createTextTexture(fontPath, fontSizes.menuButtons, "START", visualConfig.YELLOW),
        resourceManager->createTextTexture(fontPath, fontSizes.quitButton, "QUIT", visualConfig.YELLOW),
        resourceManager->loadTexture(getAssetPath(assetPaths.menuCatTexture)),
        resourceManager->loadTexture(getAssetPath(assetPaths.selectCatTexture))
    };
    
    for (size_t i = 0; i < menuAssets.size(); ++i) {
        ASSERT_NE(menuAssets[i], nullptr) << "Menu asset " << i << " failed to load";
    }
    
    // Simulate gameplay asset loading
    std::vector<SDL_Texture*> gameplayAssets = {
        resourceManager->loadTexture(getAssetPath(assetPaths.blueFishTexture)),
        resourceManager->loadTexture(getAssetPath(assetPaths.greenFishTexture)),
        resourceManager->loadTexture(getAssetPath(assetPaths.goldFishTexture)),
        resourceManager->createTextTexture(fontPath, fontSizes.hitFeedback, "1000", visualConfig.RED),
        resourceManager->createTextTexture(fontPath, fontSizes.hitFeedback, "500", visualConfig.RED),
        resourceManager->loadTexture(getAssetPath(assetPaths.oceanTexture)),
        resourceManager->loadTexture(getAssetPath(assetPaths.boatTexture)),
        resourceManager->loadTexture(getAssetPath(assetPaths.fisherTexture)),
        resourceManager->loadTexture(getAssetPath(assetPaths.hookTexture)),
        resourceManager->createTextTexture(fontPath, fontSizes.gameScore, "SCORE", visualConfig.YELLOW),
        resourceManager->createTextTexture(fontPath, fontSizes.gameNumbers, "000000", visualConfig.YELLOW)
    };
    
    for (size_t i = 0; i < gameplayAssets.size(); ++i) {
        ASSERT_NE(gameplayAssets[i], nullptr) << "Gameplay asset " << i << " failed to load";
    }
    
    // Simulate end screen asset loading
    std::vector<SDL_Texture*> endScreenAssets = {
        resourceManager->createTextTexture(fontPath, fontSizes.gameStats, "GAME STATS", visualConfig.YELLOW),
        resourceManager->createTextTexture(fontPath, fontSizes.gameStats, "HITS", visualConfig.YELLOW),
        resourceManager->createTextTexture(fontPath, fontSizes.gameStats, "ACCURACY", visualConfig.YELLOW),
        resourceManager->createTextTexture(fontPath, fontSizes.gameStats, "MISSES", visualConfig.YELLOW),
        resourceManager->createTextTexture(fontPath, fontSizes.menuButtons, "RETRY", visualConfig.YELLOW)
    };
    
    for (size_t i = 0; i < endScreenAssets.size(); ++i) {
        ASSERT_NE(endScreenAssets[i], nullptr) << "End screen asset " << i << " failed to load";
    }
    
    // Verify that repeated loading returns cached assets (simulating multiple game sessions)
    SDL_Texture* cachedMenuCat = resourceManager->loadTexture(getAssetPath(assetPaths.menuCatTexture));
    EXPECT_EQ(menuAssets[3], cachedMenuCat) << "Menu cat should be cached";
    
    SDL_Texture* cachedStartText = resourceManager->createTextTexture(
        fontPath, fontSizes.menuButtons, "START", visualConfig.YELLOW
    );
    EXPECT_EQ(menuAssets[1], cachedStartText) << "START text should be cached";
}