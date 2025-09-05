#include <gtest/gtest.h>
#include <SDL.h>
#include <SDL_image.h>
#include "Entity.hpp"

// Test fixture for Entity tests that handles SDL initialization
class EntityTest : public ::testing::Test {
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

// Test Entity construction with valid texture
TEST_F(EntityTest, ConstructorWithValidTexture) {
    testTexture = createTestTexture(64, 48);
    ASSERT_NE(testTexture, nullptr);
    
    Entity entity(10.5f, 20.3f, testTexture);
    
    EXPECT_FLOAT_EQ(entity.getX(), 10.5f);
    EXPECT_FLOAT_EQ(entity.getY(), 20.3f);
    EXPECT_EQ(entity.getTexture(), testTexture);
    
    SDL_Rect frame = entity.getCurrentFrame();
    EXPECT_EQ(frame.x, 0);
    EXPECT_EQ(frame.y, 0);
    EXPECT_EQ(frame.w, 64);  // Should match texture width
    EXPECT_EQ(frame.h, 48);  // Should match texture height
}

// Test Entity construction with null texture
TEST_F(EntityTest, ConstructorWithNullTexture) {
    Entity entity(15.7f, 25.9f, nullptr);
    
    EXPECT_FLOAT_EQ(entity.getX(), 15.7f);
    EXPECT_FLOAT_EQ(entity.getY(), 25.9f);
    EXPECT_EQ(entity.getTexture(), nullptr);
    
    SDL_Rect frame = entity.getCurrentFrame();
    EXPECT_EQ(frame.x, 0);
    EXPECT_EQ(frame.y, 0);
    EXPECT_EQ(frame.w, 0);  // Should be 0 for null texture
    EXPECT_EQ(frame.h, 0);  // Should be 0 for null texture
}

// Test Entity construction with zero coordinates
TEST_F(EntityTest, ConstructorWithZeroCoordinates) {
    testTexture = createTestTexture(32, 32);
    ASSERT_NE(testTexture, nullptr);
    
    Entity entity(0.0f, 0.0f, testTexture);
    
    EXPECT_FLOAT_EQ(entity.getX(), 0.0f);
    EXPECT_FLOAT_EQ(entity.getY(), 0.0f);
    EXPECT_EQ(entity.getTexture(), testTexture);
}

// Test Entity construction with negative coordinates
TEST_F(EntityTest, ConstructorWithNegativeCoordinates) {
    testTexture = createTestTexture(16, 16);
    ASSERT_NE(testTexture, nullptr);
    
    Entity entity(-10.5f, -20.3f, testTexture);
    
    EXPECT_FLOAT_EQ(entity.getX(), -10.5f);
    EXPECT_FLOAT_EQ(entity.getY(), -20.3f);
    EXPECT_EQ(entity.getTexture(), testTexture);
}

// Test setTexture functionality
TEST_F(EntityTest, SetTexture) {
    Entity entity(5.0f, 10.0f, nullptr);
    EXPECT_EQ(entity.getTexture(), nullptr);
    
    testTexture = createTestTexture(128, 96);
    ASSERT_NE(testTexture, nullptr);
    
    entity.setTexture(testTexture);
    EXPECT_EQ(entity.getTexture(), testTexture);
    
    // Setting to nullptr should also work
    entity.setTexture(nullptr);
    EXPECT_EQ(entity.getTexture(), nullptr);
}

// Test getCurrentFrame returns copy, not reference
TEST_F(EntityTest, GetCurrentFrameReturnsCopy) {
    testTexture = createTestTexture(50, 60);
    ASSERT_NE(testTexture, nullptr);
    
    Entity entity(0.0f, 0.0f, testTexture);
    
    SDL_Rect frame1 = entity.getCurrentFrame();
    SDL_Rect frame2 = entity.getCurrentFrame();
    
    // Should be equal values
    EXPECT_EQ(frame1.x, frame2.x);
    EXPECT_EQ(frame1.y, frame2.y);
    EXPECT_EQ(frame1.w, frame2.w);
    EXPECT_EQ(frame1.h, frame2.h);
    
    // But different memory addresses (copies)
    EXPECT_NE(&frame1, &frame2);
    
    // Modifying one shouldn't affect the other
    frame1.w = 999;
    frame2 = entity.getCurrentFrame();
    EXPECT_NE(frame1.w, frame2.w);
    EXPECT_EQ(frame2.w, 50);
}

// Test setCurrentFrameW
TEST_F(EntityTest, SetCurrentFrameW) {
    testTexture = createTestTexture(32, 32);
    ASSERT_NE(testTexture, nullptr);
    
    Entity entity(0.0f, 0.0f, testTexture);
    
    SDL_Rect originalFrame = entity.getCurrentFrame();
    EXPECT_EQ(originalFrame.w, 32);
    
    entity.setCurrentFrameW(64);
    SDL_Rect newFrame = entity.getCurrentFrame();
    EXPECT_EQ(newFrame.w, 64);
    EXPECT_EQ(newFrame.h, 32);  // Height should remain unchanged
    EXPECT_EQ(newFrame.x, 0);   // X should remain unchanged
    EXPECT_EQ(newFrame.y, 0);   // Y should remain unchanged
}

// Test setCurrentFrameH
TEST_F(EntityTest, SetCurrentFrameH) {
    testTexture = createTestTexture(32, 32);
    ASSERT_NE(testTexture, nullptr);
    
    Entity entity(0.0f, 0.0f, testTexture);
    
    SDL_Rect originalFrame = entity.getCurrentFrame();
    EXPECT_EQ(originalFrame.h, 32);
    
    entity.setCurrentFrameH(128);
    SDL_Rect newFrame = entity.getCurrentFrame();
    EXPECT_EQ(newFrame.h, 128);
    EXPECT_EQ(newFrame.w, 32);  // Width should remain unchanged
    EXPECT_EQ(newFrame.x, 0);   // X should remain unchanged
    EXPECT_EQ(newFrame.y, 0);   // Y should remain unchanged
}

// Test setting frame dimensions to zero
TEST_F(EntityTest, SetFrameDimensionsToZero) {
    testTexture = createTestTexture(100, 100);
    ASSERT_NE(testTexture, nullptr);
    
    Entity entity(0.0f, 0.0f, testTexture);
    
    entity.setCurrentFrameW(0);
    entity.setCurrentFrameH(0);
    
    SDL_Rect frame = entity.getCurrentFrame();
    EXPECT_EQ(frame.w, 0);
    EXPECT_EQ(frame.h, 0);
}

// Test setting negative frame dimensions
TEST_F(EntityTest, SetNegativeFrameDimensions) {
    testTexture = createTestTexture(50, 50);
    ASSERT_NE(testTexture, nullptr);
    
    Entity entity(0.0f, 0.0f, testTexture);
    
    entity.setCurrentFrameW(-10);
    entity.setCurrentFrameH(-20);
    
    SDL_Rect frame = entity.getCurrentFrame();
    EXPECT_EQ(frame.w, -10);  // Should accept negative values
    EXPECT_EQ(frame.h, -20);  // Should accept negative values
}

// Test multiple frame dimension changes
TEST_F(EntityTest, MultipleFrameDimensionChanges) {
    testTexture = createTestTexture(25, 25);
    ASSERT_NE(testTexture, nullptr);
    
    Entity entity(0.0f, 0.0f, testTexture);
    
    // Test multiple width changes
    entity.setCurrentFrameW(50);
    EXPECT_EQ(entity.getCurrentFrame().w, 50);
    
    entity.setCurrentFrameW(75);
    EXPECT_EQ(entity.getCurrentFrame().w, 75);
    
    entity.setCurrentFrameW(100);
    EXPECT_EQ(entity.getCurrentFrame().w, 100);
    
    // Test multiple height changes
    entity.setCurrentFrameH(30);
    EXPECT_EQ(entity.getCurrentFrame().h, 30);
    
    entity.setCurrentFrameH(60);
    EXPECT_EQ(entity.getCurrentFrame().h, 60);
    
    entity.setCurrentFrameH(90);
    EXPECT_EQ(entity.getCurrentFrame().h, 90);
    
    // Final state check
    SDL_Rect finalFrame = entity.getCurrentFrame();
    EXPECT_EQ(finalFrame.w, 100);
    EXPECT_EQ(finalFrame.x, 0);
    EXPECT_EQ(finalFrame.y, 0);
    EXPECT_EQ(finalFrame.h, 90);
}

// Test with different texture sizes
TEST_F(EntityTest, DifferentTextureSizes) {
    // Test very small texture
    SDL_Texture* smallTexture = createTestTexture(1, 1);
    ASSERT_NE(smallTexture, nullptr);
    
    Entity smallEntity(0.0f, 0.0f, smallTexture);
    SDL_Rect smallFrame = smallEntity.getCurrentFrame();
    EXPECT_EQ(smallFrame.w, 1);
    EXPECT_EQ(smallFrame.h, 1);
    
    SDL_DestroyTexture(smallTexture);
    
    // Test large texture
    SDL_Texture* largeTexture = createTestTexture(512, 256);
    ASSERT_NE(largeTexture, nullptr);
    
    Entity largeEntity(0.0f, 0.0f, largeTexture);
    SDL_Rect largeFrame = largeEntity.getCurrentFrame();
    EXPECT_EQ(largeFrame.w, 512);
    EXPECT_EQ(largeFrame.h, 256);
    
    SDL_DestroyTexture(largeTexture);
    
    // Test non-square texture
    SDL_Texture* rectTexture = createTestTexture(200, 50);
    ASSERT_NE(rectTexture, nullptr);
    
    Entity rectEntity(0.0f, 0.0f, rectTexture);
    SDL_Rect rectFrame = rectEntity.getCurrentFrame();
    EXPECT_EQ(rectFrame.w, 200);
    EXPECT_EQ(rectFrame.h, 50);
    
    SDL_DestroyTexture(rectTexture);
}

// Test Entity with floating point precision
TEST_F(EntityTest, FloatingPointPrecision) {
    testTexture = createTestTexture(32, 32);
    ASSERT_NE(testTexture, nullptr);
    
    float preciseX = 10.123456789f;
    float preciseY = 20.987654321f;
    
    Entity entity(preciseX, preciseY, testTexture);
    
    // Should maintain reasonable floating point precision
    EXPECT_NEAR(entity.getX(), preciseX, 0.000001f);
    EXPECT_NEAR(entity.getY(), preciseY, 0.000001f);
}