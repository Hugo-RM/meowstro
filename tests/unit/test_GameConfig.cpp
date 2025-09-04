#include <gtest/gtest.h>
#include "GameConfig.hpp"

// Test fixture for GameConfig tests
class GameConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Get reference to the singleton instance
        config = &GameConfig::getInstance();
    }
    
    GameConfig* config;
};

// Test singleton pattern - should return same instance
TEST_F(GameConfigTest, SingletonPattern) {
    GameConfig& instance1 = GameConfig::getInstance();
    GameConfig& instance2 = GameConfig::getInstance();
    
    // Should be the same instance
    EXPECT_EQ(&instance1, &instance2);
    EXPECT_EQ(config, &instance1);
}

// Test WindowConfig default values
TEST_F(GameConfigTest, WindowConfigDefaults) {
    const auto& windowConfig = config->getWindowConfig();
    
    EXPECT_STREQ(windowConfig.title, "Meowstro");
    EXPECT_EQ(windowConfig.width, 1920);
    EXPECT_EQ(windowConfig.height, 1080);
    EXPECT_EQ(windowConfig.flags, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
}

// Test AudioConfig default values
TEST_F(GameConfigTest, AudioConfigDefaults) {
    const auto& audioConfig = config->getAudioConfig();
    
    EXPECT_EQ(audioConfig.bpm, 147);
    EXPECT_DOUBLE_EQ(audioConfig.travelDuration, 2000.0);
    EXPECT_EQ(audioConfig.backgroundMusicPath, "./assets/audio/meowstro_short_ver.mp3");
}

// Test VisualConfig default values
TEST_F(GameConfigTest, VisualConfigDefaults) {
    const auto& visualConfig = config->getVisualConfig();
    
    // Test colors
    EXPECT_EQ(visualConfig.YELLOW.r, 255);
    EXPECT_EQ(visualConfig.YELLOW.g, 255);
    EXPECT_EQ(visualConfig.YELLOW.b, 100);
    EXPECT_EQ(visualConfig.YELLOW.a, 255);
    
    EXPECT_EQ(visualConfig.BLACK.r, 0);
    EXPECT_EQ(visualConfig.BLACK.g, 0);
    EXPECT_EQ(visualConfig.BLACK.b, 0);
    EXPECT_EQ(visualConfig.BLACK.a, 255);
    
    EXPECT_EQ(visualConfig.RED.r, 255);
    EXPECT_EQ(visualConfig.RED.g, 0);
    EXPECT_EQ(visualConfig.RED.b, 0);
    EXPECT_EQ(visualConfig.RED.a, 255);
    
    EXPECT_EQ(visualConfig.frameDelay, 75);
}

// Test AssetPaths default values
TEST_F(GameConfigTest, AssetPathsDefaults) {
    const auto& assetPaths = config->getAssetPaths();
    
    // Font path
    EXPECT_EQ(assetPaths.fontPath, "./assets/fonts/Comic Sans MS.ttf");
    
    // Main textures
    EXPECT_EQ(assetPaths.oceanTexture, "./assets/images/Ocean.png");
    EXPECT_EQ(assetPaths.boatTexture, "./assets/images/boat.png");
    EXPECT_EQ(assetPaths.fisherTexture, "./assets/images/fisher.png");
    EXPECT_EQ(assetPaths.hookTexture, "./assets/images/hook.png");
    EXPECT_EQ(assetPaths.menuCatTexture, "./assets/images/menu_cat.png");
    EXPECT_EQ(assetPaths.selectCatTexture, "./assets/images/select_cat.png");
    
    // Fish textures
    EXPECT_EQ(assetPaths.blueFishTexture, "./assets/images/blue_fish.png");
    EXPECT_EQ(assetPaths.greenFishTexture, "./assets/images/green_fish.png");
    EXPECT_EQ(assetPaths.goldFishTexture, "./assets/images/gold_fish.png");
}

// Test GameplayConfig default values
TEST_F(GameConfigTest, GameplayConfigDefaults) {
    const auto& gameplayConfig = config->getGameplayConfig();
    
    EXPECT_EQ(gameplayConfig.numBeats, 25);
    EXPECT_EQ(gameplayConfig.numFishTextures, 3);
    EXPECT_EQ(gameplayConfig.throwDuration, 200);
    EXPECT_EQ(gameplayConfig.hookTargetX, 650);
    EXPECT_EQ(gameplayConfig.hookTargetY, 625);
    EXPECT_EQ(gameplayConfig.fishTargetX, 660);
    
    // Test fish start locations
    const auto& fishLocations = gameplayConfig.fishStartXLocations;
    EXPECT_EQ(fishLocations.size(), 25);
    
    // Test some specific locations
    EXPECT_EQ(fishLocations[0], 1352);
    EXPECT_EQ(fishLocations[1], 2350);
    EXPECT_EQ(fishLocations[24], 10160); // Last element
    
    // Test that all values are positive
    for (int location : fishLocations) {
        EXPECT_GT(location, 0);
    }
    
    // Test that locations are generally increasing (rough check)
    EXPECT_LT(fishLocations[0], fishLocations[24]);
}

// Test FontSizes default values
TEST_F(GameConfigTest, FontSizesDefaults) {
    const auto& fontSizes = config->getFontSizes();
    
    EXPECT_EQ(fontSizes.menuLogo, 75);
    EXPECT_EQ(fontSizes.menuButtons, 55);
    EXPECT_EQ(fontSizes.quitButton, 65);
    EXPECT_EQ(fontSizes.gameScore, 40);
    EXPECT_EQ(fontSizes.gameNumbers, 35);
    EXPECT_EQ(fontSizes.gameStats, 55);
    EXPECT_EQ(fontSizes.hitFeedback, 30);
    
    // Test that all font sizes are positive
    EXPECT_GT(fontSizes.menuLogo, 0);
    EXPECT_GT(fontSizes.menuButtons, 0);
    EXPECT_GT(fontSizes.quitButton, 0);
    EXPECT_GT(fontSizes.gameScore, 0);
    EXPECT_GT(fontSizes.gameNumbers, 0);
    EXPECT_GT(fontSizes.gameStats, 0);
    EXPECT_GT(fontSizes.hitFeedback, 0);
}

// Test beat timing initialization
TEST_F(GameConfigTest, BeatTimingInitialization) {
    // Before initialization, noteBeats should be empty
    const auto& gameplayConfig = config->getGameplayConfig();
    
    // Initialize beat timings
    config->initializeBeatTimings();
    
    // After initialization, should have 25 beats
    const auto& noteBeats = gameplayConfig.noteBeats;
    EXPECT_EQ(noteBeats.size(), 25);
    
    // Test that all beat times are positive
    for (double beat : noteBeats) {
        EXPECT_GT(beat, 0.0);
    }
    
    // Test that beat times are generally increasing
    for (size_t i = 1; i < noteBeats.size(); ++i) {
        EXPECT_GE(noteBeats[i], noteBeats[i-1]);
    }
}

// Test multiple calls to initializeBeatTimings doesn't reinitialize
TEST_F(GameConfigTest, BeatTimingInitializationIdempotent) {
    config->initializeBeatTimings();
    
    const auto& gameplayConfig = config->getGameplayConfig();
    const auto& noteBeats = gameplayConfig.noteBeats;
    
    // Store the first beat time
    double firstBeat = noteBeats[0];
    
    // Call initialization again
    config->initializeBeatTimings();
    
    // Should still have same values
    EXPECT_EQ(noteBeats.size(), 25);
    EXPECT_DOUBLE_EQ(noteBeats[0], firstBeat);
}

// Test consistency between numBeats and fish locations
TEST_F(GameConfigTest, ConsistencyBetweenBeatsAndFishLocations) {
    const auto& gameplayConfig = config->getGameplayConfig();
    
    EXPECT_EQ(gameplayConfig.numBeats, gameplayConfig.fishStartXLocations.size());
    
    config->initializeBeatTimings();
    EXPECT_EQ(gameplayConfig.numBeats, gameplayConfig.noteBeats.size());
    EXPECT_EQ(gameplayConfig.fishStartXLocations.size(), gameplayConfig.noteBeats.size());
}

// Test that configuration values are reasonable for a game
TEST_F(GameConfigTest, ReasonableConfigurationValues) {
    const auto& windowConfig = config->getWindowConfig();
    const auto& audioConfig = config->getAudioConfig();
    const auto& visualConfig = config->getVisualConfig();
    const auto& gameplayConfig = config->getGameplayConfig();
    
    // Window dimensions should be reasonable
    EXPECT_GE(windowConfig.width, 800);
    EXPECT_GE(windowConfig.height, 600);
    EXPECT_LE(windowConfig.width, 4000);
    EXPECT_LE(windowConfig.height, 4000);
    
    // BPM should be reasonable for music
    EXPECT_GE(audioConfig.bpm, 60);
    EXPECT_LE(audioConfig.bpm, 300);
    
    // Travel duration should be positive
    EXPECT_GT(audioConfig.travelDuration, 0.0);
    
    // Frame delay should be reasonable
    EXPECT_GE(visualConfig.frameDelay, 1);
    EXPECT_LE(visualConfig.frameDelay, 1000);
    
    // Game mechanics should be reasonable
    EXPECT_GT(gameplayConfig.numBeats, 0);
    EXPECT_LE(gameplayConfig.numBeats, 100);
    EXPECT_GT(gameplayConfig.numFishTextures, 0);
    EXPECT_GT(gameplayConfig.throwDuration, 0);
}

// Test path strings are not empty
TEST_F(GameConfigTest, PathsNotEmpty) {
    const auto& assetPaths = config->getAssetPaths();
    const auto& audioConfig = config->getAudioConfig();
    
    EXPECT_FALSE(assetPaths.fontPath.empty());
    EXPECT_FALSE(assetPaths.oceanTexture.empty());
    EXPECT_FALSE(assetPaths.boatTexture.empty());
    EXPECT_FALSE(assetPaths.fisherTexture.empty());
    EXPECT_FALSE(assetPaths.hookTexture.empty());
    EXPECT_FALSE(assetPaths.menuCatTexture.empty());
    EXPECT_FALSE(assetPaths.selectCatTexture.empty());
    EXPECT_FALSE(assetPaths.blueFishTexture.empty());
    EXPECT_FALSE(assetPaths.greenFishTexture.empty());
    EXPECT_FALSE(assetPaths.goldFishTexture.empty());
    EXPECT_FALSE(audioConfig.backgroundMusicPath.empty());
}

// Test SDL color values are within valid range
TEST_F(GameConfigTest, ColorValuesValid) {
    const auto& visualConfig = config->getVisualConfig();
    
    // Test YELLOW color
    EXPECT_GE(visualConfig.YELLOW.r, 0);
    EXPECT_LE(visualConfig.YELLOW.r, 255);
    EXPECT_GE(visualConfig.YELLOW.g, 0);
    EXPECT_LE(visualConfig.YELLOW.g, 255);
    EXPECT_GE(visualConfig.YELLOW.b, 0);
    EXPECT_LE(visualConfig.YELLOW.b, 255);
    EXPECT_GE(visualConfig.YELLOW.a, 0);
    EXPECT_LE(visualConfig.YELLOW.a, 255);
    
    // Test BLACK color
    EXPECT_GE(visualConfig.BLACK.r, 0);
    EXPECT_LE(visualConfig.BLACK.r, 255);
    EXPECT_GE(visualConfig.BLACK.g, 0);
    EXPECT_LE(visualConfig.BLACK.g, 255);
    EXPECT_GE(visualConfig.BLACK.b, 0);
    EXPECT_LE(visualConfig.BLACK.b, 255);
    EXPECT_GE(visualConfig.BLACK.a, 0);
    EXPECT_LE(visualConfig.BLACK.a, 255);
    
    // Test RED color
    EXPECT_GE(visualConfig.RED.r, 0);
    EXPECT_LE(visualConfig.RED.r, 255);
    EXPECT_GE(visualConfig.RED.g, 0);
    EXPECT_LE(visualConfig.RED.g, 255);
    EXPECT_GE(visualConfig.RED.b, 0);
    EXPECT_LE(visualConfig.RED.b, 255);
    EXPECT_GE(visualConfig.RED.a, 0);
    EXPECT_LE(visualConfig.RED.a, 255);
}