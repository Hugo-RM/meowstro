#include <gtest/gtest.h>
#include <SDL.h>
#include <SDL_image.h>
#include "Sprite.hpp"

// Test fixture for Sprite tests that handles SDL initialization
class SpriteTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize SDL for texture operations
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            FAIL() << "SDL_Init failed: " << SDL_GetError();
        }
        
        if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
            SDL_Quit();
            FAIL() << "IMG_Init failed: " << IMG_GetError();
        }
        
        // Create a minimal window and renderer for texture operations
        window = SDL_CreateWindow("Test Window", 
                                SDL_WINDOWPOS_UNDEFINED, 
                                SDL_WINDOWPOS_UNDEFINED, 
                                100, 100, 
                                SDL_WINDOW_HIDDEN);
        if (!window) {
            IMG_Quit();
            SDL_Quit();
            FAIL() << "SDL_CreateWindow failed: " << SDL_GetError();
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        if (!renderer) {
            SDL_DestroyWindow(window);
            IMG_Quit();
            SDL_Quit();
            FAIL() << "SDL_CreateRenderer failed: " << SDL_GetError();
        }
    }
    
    void TearDown() override {
        if (testTexture) {
            SDL_DestroyTexture(testTexture);
        }
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
        IMG_Quit();
        SDL_Quit();
    }
    
    // Helper function to create a test texture
    SDL_Texture* createTestTexture(int width, int height) {
        SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        if (!surface) return nullptr;
        
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    }
    
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* testTexture = nullptr;
};

// Test basic Sprite construction
TEST_F(SpriteTest, BasicConstruction) {
    testTexture = createTestTexture(100, 80); // 4x2 grid with 25x40 frames
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(10.5f, 20.3f, testTexture, 2, 4);
    
    // Test inherited Entity properties
    EXPECT_FLOAT_EQ(sprite.getX(), 10.5f);
    EXPECT_FLOAT_EQ(sprite.getY(), 20.3f);
    EXPECT_EQ(sprite.getTexture(), testTexture);
    
    // Test Sprite-specific properties
    EXPECT_EQ(sprite.getRow(), 1);  // Should start at (1,1)
    EXPECT_EQ(sprite.getCol(), 1);
    
    // Test frame calculations (100/4 = 25 width, 80/2 = 40 height)
    SDL_Rect frame = sprite.getCurrentFrame();
    EXPECT_EQ(frame.w, 25);
    EXPECT_EQ(frame.h, 40);
    EXPECT_EQ(frame.x, 0);  // First frame starts at (0,0)
    EXPECT_EQ(frame.y, 0);
}

// Test Sprite construction with single frame
TEST_F(SpriteTest, SingleFrameConstruction) {
    testTexture = createTestTexture(64, 64);
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(0.0f, 0.0f, testTexture, 1, 1);
    
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 1);
    
    SDL_Rect frame = sprite.getCurrentFrame();
    EXPECT_EQ(frame.w, 64);  // Full texture width
    EXPECT_EQ(frame.h, 64);  // Full texture height
    EXPECT_EQ(frame.x, 0);
    EXPECT_EQ(frame.y, 0);
}

// Test setFrame functionality
TEST_F(SpriteTest, SetFrameValid) {
    testTexture = createTestTexture(120, 90); // 3x2 grid, 40x45 frames
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(0.0f, 0.0f, testTexture, 2, 3);
    
    // Test setting valid frames
    sprite.setFrame(1, 2);
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 2);
    
    SDL_Rect frame = sprite.getCurrentFrame();
    EXPECT_EQ(frame.x, 40);  // Second column: (2-1) * 40
    EXPECT_EQ(frame.y, 0);   // First row: (1-1) * 45
    
    sprite.setFrame(2, 3);
    EXPECT_EQ(sprite.getRow(), 2);
    EXPECT_EQ(sprite.getCol(), 3);
    
    frame = sprite.getCurrentFrame();
    EXPECT_EQ(frame.x, 80);  // Third column: (3-1) * 40
    EXPECT_EQ(frame.y, 45);  // Second row: (2-1) * 45
}

// Test setFrame with invalid values
TEST_F(SpriteTest, SetFrameInvalid) {
    testTexture = createTestTexture(60, 40); // 2x3 grid
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(0.0f, 0.0f, testTexture, 2, 3);
    
    // Set to valid frame first
    sprite.setFrame(2, 2);
    EXPECT_EQ(sprite.getRow(), 2);
    EXPECT_EQ(sprite.getCol(), 2);
    
    // Try invalid values - should not change
    sprite.setFrame(0, 1);  // Row 0 is invalid
    EXPECT_EQ(sprite.getRow(), 2);
    EXPECT_EQ(sprite.getCol(), 2);
    
    sprite.setFrame(1, 0);  // Col 0 is invalid
    EXPECT_EQ(sprite.getRow(), 2);
    EXPECT_EQ(sprite.getCol(), 2);
    
    sprite.setFrame(3, 1);  // Row 3 is invalid (maxRow = 2)
    EXPECT_EQ(sprite.getRow(), 2);
    EXPECT_EQ(sprite.getCol(), 2);
    
    sprite.setFrame(1, 4);  // Col 4 is invalid (maxCol = 3)
    EXPECT_EQ(sprite.getRow(), 2);
    EXPECT_EQ(sprite.getCol(), 2);
    
    sprite.setFrame(-1, -1);  // Negative values are invalid
    EXPECT_EQ(sprite.getRow(), 2);
    EXPECT_EQ(sprite.getCol(), 2);
}

// Test resetFrame functionality
TEST_F(SpriteTest, ResetFrame) {
    testTexture = createTestTexture(80, 60);
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(0.0f, 0.0f, testTexture, 3, 4);
    
    // Move to different frame
    sprite.setFrame(3, 4);
    EXPECT_EQ(sprite.getRow(), 3);
    EXPECT_EQ(sprite.getCol(), 4);
    
    // Reset should go back to (1,1)
    sprite.resetFrame();
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 1);
    
    SDL_Rect frame = sprite.getCurrentFrame();
    EXPECT_EQ(frame.x, 0);
    EXPECT_EQ(frame.y, 0);
}

// Test movement methods
TEST_F(SpriteTest, MovementMethods) {
    testTexture = createTestTexture(32, 32);
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(100.0f, 200.0f, testTexture, 1, 1);
    
    // Test setLoc
    sprite.setLoc(50, 75);
    EXPECT_FLOAT_EQ(sprite.getX(), 50.0f);
    EXPECT_FLOAT_EQ(sprite.getY(), 75.0f);
    
    // Test moveRight
    sprite.moveRight(25);
    EXPECT_FLOAT_EQ(sprite.getX(), 75.0f);
    EXPECT_FLOAT_EQ(sprite.getY(), 75.0f);
    
    // Test moveLeft
    sprite.moveLeft(10);
    EXPECT_FLOAT_EQ(sprite.getX(), 65.0f);
    EXPECT_FLOAT_EQ(sprite.getY(), 75.0f);
    
    // Test moveDown
    sprite.moveDown(20);
    EXPECT_FLOAT_EQ(sprite.getX(), 65.0f);
    EXPECT_FLOAT_EQ(sprite.getY(), 95.0f);
    
    // Test moveUp
    sprite.moveUp(15);
    EXPECT_FLOAT_EQ(sprite.getX(), 65.0f);
    EXPECT_FLOAT_EQ(sprite.getY(), 80.0f);
}

// Test movement with negative values
TEST_F(SpriteTest, MovementWithNegativeValues) {
    testTexture = createTestTexture(16, 16);
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(50.0f, 50.0f, testTexture, 1, 1);
    
    // Move with negative deltas (should still move in specified direction)
    sprite.moveRight(-10);  // Move left
    EXPECT_FLOAT_EQ(sprite.getX(), 40.0f);
    
    sprite.moveLeft(-5);    // Move right
    EXPECT_FLOAT_EQ(sprite.getX(), 45.0f);
    
    sprite.moveDown(-20);   // Move up
    EXPECT_FLOAT_EQ(sprite.getY(), 30.0f);
    
    sprite.moveUp(-10);     // Move down
    EXPECT_FLOAT_EQ(sprite.getY(), 40.0f);
}

// Test post-increment operator
TEST_F(SpriteTest, PostIncrementOperator) {
    testTexture = createTestTexture(90, 60); // 3x2 grid
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(0.0f, 0.0f, testTexture, 2, 3);
    
    // Should start at (1,1)
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 1);
    
    // First increment: (1,1) -> (2,1)
    Sprite oldSprite = sprite++;
    EXPECT_EQ(oldSprite.getRow(), 1);  // Returned value should be old
    EXPECT_EQ(oldSprite.getCol(), 1);
    EXPECT_EQ(sprite.getRow(), 2);     // Current should be new
    EXPECT_EQ(sprite.getCol(), 1);
    
    // Second increment: (2,1) -> (1,2) (wrap row, advance column)
    sprite++;
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 2);
    
    // Third increment: (1,2) -> (2,2)
    sprite++;
    EXPECT_EQ(sprite.getRow(), 2);
    EXPECT_EQ(sprite.getCol(), 2);
    
    // Fourth increment: (2,2) -> (1,3) (wrap row, advance column)
    sprite++;
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 3);
    
    // Fifth increment: (1,3) -> (2,3)
    sprite++;
    EXPECT_EQ(sprite.getRow(), 2);
    EXPECT_EQ(sprite.getCol(), 3);
    
    // Sixth increment: (2,3) -> (1,1) (wrap both row and column)
    sprite++;
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 1);
}

// Test post-increment with single row
TEST_F(SpriteTest, PostIncrementSingleRow) {
    testTexture = createTestTexture(120, 30); // 1x4 grid
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(0.0f, 0.0f, testTexture, 1, 4);
    
    // Should advance columns only
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 1);
    
    sprite++;
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 2);
    
    sprite++;
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 3);
    
    sprite++;
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 4);
    
    sprite++;  // Should wrap to (1,1)
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 1);
}

// Test post-increment with single column
TEST_F(SpriteTest, PostIncrementSingleColumn) {
    testTexture = createTestTexture(40, 160); // 4x1 grid
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(0.0f, 0.0f, testTexture, 4, 1);
    
    // Should advance rows, then wrap to (1,1)
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 1);
    
    sprite++;
    EXPECT_EQ(sprite.getRow(), 2);
    EXPECT_EQ(sprite.getCol(), 1);
    
    sprite++;
    EXPECT_EQ(sprite.getRow(), 3);
    EXPECT_EQ(sprite.getCol(), 1);
    
    sprite++;
    EXPECT_EQ(sprite.getRow(), 4);
    EXPECT_EQ(sprite.getCol(), 1);
    
    sprite++;  // Should wrap to (1,1) and advance column, but col wraps too
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 1);
}

// Test frame calculations with various grid sizes
TEST_F(SpriteTest, FrameCalculationsVariousGrids) {
    // Test 2x2 grid with 100x100 texture
    SDL_Texture* texture2x2 = createTestTexture(100, 100);
    ASSERT_NE(texture2x2, nullptr);
    
    Sprite sprite2x2(0.0f, 0.0f, texture2x2, 2, 2);
    sprite2x2.setFrame(2, 2);  // Bottom-right frame
    
    SDL_Rect frame = sprite2x2.getCurrentFrame();
    EXPECT_EQ(frame.w, 50);   // 100/2
    EXPECT_EQ(frame.h, 50);   // 100/2
    EXPECT_EQ(frame.x, 50);   // (2-1) * 50
    EXPECT_EQ(frame.y, 50);   // (2-1) * 50
    
    SDL_DestroyTexture(texture2x2);
    
    // Test 1x5 grid with 150x30 texture
    SDL_Texture* texture1x5 = createTestTexture(150, 30);
    ASSERT_NE(texture1x5, nullptr);
    
    Sprite sprite1x5(0.0f, 0.0f, texture1x5, 1, 5);
    sprite1x5.setFrame(1, 4);  // Fourth column
    
    frame = sprite1x5.getCurrentFrame();
    EXPECT_EQ(frame.w, 30);   // 150/5
    EXPECT_EQ(frame.h, 30);   // 30/1
    EXPECT_EQ(frame.x, 90);   // (4-1) * 30
    EXPECT_EQ(frame.y, 0);    // (1-1) * 30
    
    SDL_DestroyTexture(texture1x5);
}

// Test edge case with 1x1 grid
TEST_F(SpriteTest, SingleFrameGrid) {
    testTexture = createTestTexture(64, 48);
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(0.0f, 0.0f, testTexture, 1, 1);
    
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 1);
    
    SDL_Rect frame = sprite.getCurrentFrame();
    EXPECT_EQ(frame.w, 64);
    EXPECT_EQ(frame.h, 48);
    EXPECT_EQ(frame.x, 0);
    EXPECT_EQ(frame.y, 0);
    
    // Increment should stay at (1,1)
    sprite++;
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 1);
    
    // Reset should also stay at (1,1)
    sprite.resetFrame();
    EXPECT_EQ(sprite.getRow(), 1);
    EXPECT_EQ(sprite.getCol(), 1);
}

// Test that Sprite maintains Entity functionality
TEST_F(SpriteTest, InheritsEntityFunctionality) {
    testTexture = createTestTexture(32, 32);
    ASSERT_NE(testTexture, nullptr);
    
    Sprite sprite(25.5f, 35.7f, testTexture, 2, 2);
    
    // Test Entity getters
    EXPECT_FLOAT_EQ(sprite.getX(), 25.5f);
    EXPECT_FLOAT_EQ(sprite.getY(), 35.7f);
    EXPECT_EQ(sprite.getTexture(), testTexture);
    
    // Test Entity texture setting
    SDL_Texture* newTexture = createTestTexture(64, 64);
    ASSERT_NE(newTexture, nullptr);
    
    sprite.setTexture(newTexture);
    EXPECT_EQ(sprite.getTexture(), newTexture);
    
    SDL_DestroyTexture(newTexture);
}