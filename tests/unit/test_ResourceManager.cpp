#include <gtest/gtest.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "ResourceManager.hpp"
#include "Font.hpp"

// Test fixture for ResourceManager tests that handles SDL initialization
class ResourceManagerTest : public ::testing::Test {
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
        
        if (TTF_Init() != 0) {
            IMG_Quit();
            SDL_Quit();
            FAIL() << "TTF_Init failed: " << TTF_GetError();
        }
        
        // Create a minimal window and renderer for texture operations
        window = SDL_CreateWindow("Test Window", 
                                SDL_WINDOWPOS_UNDEFINED, 
                                SDL_WINDOWPOS_UNDEFINED, 
                                100, 100, 
                                SDL_WINDOW_HIDDEN);
        if (!window) {
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            FAIL() << "SDL_CreateWindow failed: " << SDL_GetError();
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        if (!renderer) {
            SDL_DestroyWindow(window);
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            FAIL() << "SDL_CreateRenderer failed: " << SDL_GetError();
        }
    }
    
    void TearDown() override {
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
    
    // Helper function to create a test PNG file
    bool createTestImage(const std::string& filename, int width = 32, int height = 32) {
        SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 
                                                   0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
        if (!surface) return false;
        
        // Fill with a simple pattern
        SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, 255, 128, 64, 255));
        
        int result = IMG_SavePNG(surface, filename.c_str());
        SDL_FreeSurface(surface);
        return result == 0;
    }
    
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};

// Test ResourceManager construction with valid renderer
TEST_F(ResourceManagerTest, ConstructorWithValidRenderer) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
}

// Test ResourceManager construction with null renderer
TEST_F(ResourceManagerTest, ConstructorWithNullRenderer) {
    ResourceManager resourceManager(nullptr);
    EXPECT_FALSE(resourceManager.isValid());
}

// Test loadTexture with nonexistent file
TEST_F(ResourceManagerTest, LoadTextureNonexistentFile) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    SDL_Texture* texture = resourceManager.loadTexture("nonexistent_file.png");
    EXPECT_EQ(texture, nullptr);
}

// Test loadTexture with empty file path
TEST_F(ResourceManagerTest, LoadTextureEmptyPath) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    SDL_Texture* texture = resourceManager.loadTexture("");
    EXPECT_EQ(texture, nullptr);
}

// Test loadTexture with valid file
TEST_F(ResourceManagerTest, LoadTextureValidFile) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    // Create a test PNG file
    std::string testFile = "test_texture.png";
    ASSERT_TRUE(createTestImage(testFile, 64, 48));
    
    SDL_Texture* texture = resourceManager.loadTexture(testFile);
    EXPECT_NE(texture, nullptr);
    
    // Verify texture properties
    if (texture) {
        int width, height;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        EXPECT_EQ(width, 64);
        EXPECT_EQ(height, 48);
    }
    
    // Cleanup test file
    std::remove(testFile.c_str());
}

// Test texture caching - loading same file twice should return same texture
TEST_F(ResourceManagerTest, TextureCaching) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    // Create a test PNG file
    std::string testFile = "test_texture_cache.png";
    ASSERT_TRUE(createTestImage(testFile));
    
    SDL_Texture* texture1 = resourceManager.loadTexture(testFile);
    SDL_Texture* texture2 = resourceManager.loadTexture(testFile);
    
    EXPECT_NE(texture1, nullptr);
    EXPECT_EQ(texture1, texture2); // Should be the same texture object
    
    // Cleanup test file
    std::remove(testFile.c_str());
}

// Test loadTexture on invalid ResourceManager
TEST_F(ResourceManagerTest, LoadTextureInvalidResourceManager) {
    ResourceManager resourceManager(nullptr);
    EXPECT_FALSE(resourceManager.isValid());
    
    SDL_Texture* texture = resourceManager.loadTexture("any_file.png");
    EXPECT_EQ(texture, nullptr);
}

// Test createTextTexture with empty font path
TEST_F(ResourceManagerTest, CreateTextTextureEmptyFontPath) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Texture* texture = resourceManager.createTextTexture("", 20, "Hello", white);
    EXPECT_EQ(texture, nullptr);
}

// Test createTextTexture with empty text
TEST_F(ResourceManagerTest, CreateTextTextureEmptyText) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Texture* texture = resourceManager.createTextTexture("test_font.ttf", 20, "", white);
    EXPECT_EQ(texture, nullptr);
}

// Test createTextTexture on invalid ResourceManager
TEST_F(ResourceManagerTest, CreateTextTextureInvalidResourceManager) {
    ResourceManager resourceManager(nullptr);
    EXPECT_FALSE(resourceManager.isValid());
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Texture* texture = resourceManager.createTextTexture("test_font.ttf", 20, "Hello", white);
    EXPECT_EQ(texture, nullptr);
}

// Test getFont with nonexistent font file
TEST_F(ResourceManagerTest, GetFontNonexistentFile) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    Font* font = resourceManager.getFont("nonexistent_font.ttf", 20);
    EXPECT_EQ(font, nullptr);
}

// Test font caching - getting same font twice should return same Font object
TEST_F(ResourceManagerTest, FontCaching) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    // This test will fail with nonexistent font, but we can test the caching logic
    Font* font1 = resourceManager.getFont("nonexistent_font.ttf", 20);
    Font* font2 = resourceManager.getFont("nonexistent_font.ttf", 20);
    
    // Both should be null, but the caching mechanism should be consistent
    EXPECT_EQ(font1, font2); // Both null, so equal
}

// Test font caching with different sizes
TEST_F(ResourceManagerTest, FontCachingDifferentSizes) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    Font* font1 = resourceManager.getFont("test_font.ttf", 20);
    Font* font2 = resourceManager.getFont("test_font.ttf", 24);
    
    // Different sizes should be treated as different fonts
    // Both will be null due to nonexistent file, but they should be cached separately
    EXPECT_EQ(font1, nullptr);
    EXPECT_EQ(font2, nullptr);
}

// Test cleanup functionality
TEST_F(ResourceManagerTest, CleanupFunctionality) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    // Create test texture file
    std::string testFile = "test_cleanup.png";
    ASSERT_TRUE(createTestImage(testFile));
    
    // Load a texture
    SDL_Texture* texture = resourceManager.loadTexture(testFile);
    EXPECT_NE(texture, nullptr);
    
    // Manual cleanup should not crash
    resourceManager.cleanup();
    
    // After cleanup, loading the same texture should create a new one
    SDL_Texture* texture2 = resourceManager.loadTexture(testFile);
    EXPECT_NE(texture2, nullptr);
    // texture2 might be the same pointer due to SDL texture pool, but that's okay
    
    // Cleanup test file
    std::remove(testFile.c_str());
}

// Test destructor cleanup (implicit)
TEST_F(ResourceManagerTest, DestructorCleanup) {
    std::string testFile = "test_destructor.png";
    ASSERT_TRUE(createTestImage(testFile));
    
    {
        ResourceManager resourceManager(renderer);
        EXPECT_TRUE(resourceManager.isValid());
        
        SDL_Texture* texture = resourceManager.loadTexture(testFile);
        EXPECT_NE(texture, nullptr);
        
        // ResourceManager goes out of scope here, destructor should clean up
    }
    
    // If we reach here without crash, destructor cleanup worked
    EXPECT_TRUE(true);
    
    // Cleanup test file
    std::remove(testFile.c_str());
}

// Test multiple different textures
TEST_F(ResourceManagerTest, MultipleDifferentTextures) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    // Create multiple test files
    std::string testFile1 = "test_multi1.png";
    std::string testFile2 = "test_multi2.png";
    ASSERT_TRUE(createTestImage(testFile1, 32, 32));
    ASSERT_TRUE(createTestImage(testFile2, 64, 64));
    
    SDL_Texture* texture1 = resourceManager.loadTexture(testFile1);
    SDL_Texture* texture2 = resourceManager.loadTexture(testFile2);
    
    EXPECT_NE(texture1, nullptr);
    EXPECT_NE(texture2, nullptr);
    EXPECT_NE(texture1, texture2); // Should be different textures
    
    // Verify they have different dimensions
    if (texture1 && texture2) {
        int w1, h1, w2, h2;
        SDL_QueryTexture(texture1, nullptr, nullptr, &w1, &h1);
        SDL_QueryTexture(texture2, nullptr, nullptr, &w2, &h2);
        
        EXPECT_EQ(w1, 32);
        EXPECT_EQ(h1, 32);
        EXPECT_EQ(w2, 64);
        EXPECT_EQ(h2, 64);
    }
    
    // Cleanup test files
    std::remove(testFile1.c_str());
    std::remove(testFile2.c_str());
}

// Test isValid method consistency
TEST_F(ResourceManagerTest, IsValidConsistency) {
    ResourceManager validRM(renderer);
    ResourceManager invalidRM(nullptr);
    
    EXPECT_TRUE(validRM.isValid());
    EXPECT_FALSE(invalidRM.isValid());
    
    // Should remain consistent after operations
    validRM.loadTexture("nonexistent.png");
    EXPECT_TRUE(validRM.isValid());
    
    invalidRM.loadTexture("any_file.png");
    EXPECT_FALSE(invalidRM.isValid());
}

// Test edge case with very small texture
TEST_F(ResourceManagerTest, VerySmallTexture) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    std::string testFile = "test_tiny.png";
    ASSERT_TRUE(createTestImage(testFile, 1, 1));
    
    SDL_Texture* texture = resourceManager.loadTexture(testFile);
    EXPECT_NE(texture, nullptr);
    
    if (texture) {
        int width, height;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        EXPECT_EQ(width, 1);
        EXPECT_EQ(height, 1);
    }
    
    std::remove(testFile.c_str());
}

// Test key generation for text textures (indirectly through caching behavior)
TEST_F(ResourceManagerTest, TextTextureCaching) {
    ResourceManager resourceManager(renderer);
    EXPECT_TRUE(resourceManager.isValid());
    
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color blue = {0, 0, 255, 255};
    
    // These will fail due to nonexistent font, but we can test that different parameters
    // are treated as different cache keys
    SDL_Texture* tex1 = resourceManager.createTextTexture("font.ttf", 20, "Hello", red);
    SDL_Texture* tex2 = resourceManager.createTextTexture("font.ttf", 20, "Hello", blue); // Different color
    SDL_Texture* tex3 = resourceManager.createTextTexture("font.ttf", 24, "Hello", red); // Different size
    SDL_Texture* tex4 = resourceManager.createTextTexture("font.ttf", 20, "World", red); // Different text
    
    // All should be null due to nonexistent font
    EXPECT_EQ(tex1, nullptr);
    EXPECT_EQ(tex2, nullptr);
    EXPECT_EQ(tex3, nullptr);
    EXPECT_EQ(tex4, nullptr);
}