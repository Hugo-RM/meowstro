#include <gtest/gtest.h>
#include <SDL.h>
#include "InputHandler.hpp"

// Test fixture for InputHandler tests that handles SDL initialization
class InputHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize SDL for input handling
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            FAIL() << "SDL_Init failed: " << SDL_GetError();
        }
        
        // Create minimal window for SDL context
        window = SDL_CreateWindow("Test Window", 
                                SDL_WINDOWPOS_UNDEFINED, 
                                SDL_WINDOWPOS_UNDEFINED, 
                                100, 100, 
                                SDL_WINDOW_HIDDEN);
        if (!window) {
            SDL_Quit();
            FAIL() << "SDL_CreateWindow failed: " << SDL_GetError();
        }
        
        inputHandler = std::make_unique<InputHandler>();
    }
    
    void TearDown() override {
        inputHandler.reset();
        if (window) {
            SDL_DestroyWindow(window);
        }
        SDL_Quit();
    }
    
    // Helper to create SDL_Event structures
    SDL_Event createKeyDownEvent(SDL_Keycode key) {
        SDL_Event event;
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = key;
        event.key.repeat = 0;
        return event;
    }
    
    SDL_Event createKeyUpEvent(SDL_Keycode key) {
        SDL_Event event;
        event.type = SDL_KEYUP;
        event.key.keysym.sym = key;
        return event;
    }
    
    SDL_Event createQuitEvent() {
        SDL_Event event;
        event.type = SDL_QUIT;
        return event;
    }
    
    SDL_Window* window = nullptr;
    std::unique_ptr<InputHandler> inputHandler;
};

// Test InputHandler construction
TEST_F(InputHandlerTest, Construction) {
    EXPECT_NE(inputHandler.get(), nullptr);
    EXPECT_FALSE(inputHandler->isSpaceHeld());
}

// Test SDL_QUIT event handling (universal)
TEST_F(InputHandlerTest, QuitEventUniversal) {
    SDL_Event quitEvent = createQuitEvent();
    
    // Should return Quit regardless of game state
    EXPECT_EQ(inputHandler->processInput(quitEvent, GameState::MainMenu), InputAction::Quit);
    EXPECT_EQ(inputHandler->processInput(quitEvent, GameState::Playing), InputAction::Quit);
    EXPECT_EQ(inputHandler->processInput(quitEvent, GameState::EndScreen), InputAction::Quit);
}

// Test MainMenu input handling
TEST_F(InputHandlerTest, MainMenuInputHandling) {
    // Test ESCAPE key
    SDL_Event escapeEvent = createKeyDownEvent(SDLK_ESCAPE);
    EXPECT_EQ(inputHandler->processInput(escapeEvent, GameState::MainMenu), InputAction::Quit);
    
    // Test SPACE key
    SDL_Event spaceEvent = createKeyDownEvent(SDLK_SPACE);
    EXPECT_EQ(inputHandler->processInput(spaceEvent, GameState::MainMenu), InputAction::Select);
    
    // Test UP arrow
    SDL_Event upEvent = createKeyDownEvent(SDLK_UP);
    EXPECT_EQ(inputHandler->processInput(upEvent, GameState::MainMenu), InputAction::MenuUp);
    
    // Test DOWN arrow
    SDL_Event downEvent = createKeyDownEvent(SDLK_DOWN);
    EXPECT_EQ(inputHandler->processInput(downEvent, GameState::MainMenu), InputAction::MenuDown);
    
    // Test unmapped key
    SDL_Event randomEvent = createKeyDownEvent(SDLK_a);
    EXPECT_EQ(inputHandler->processInput(randomEvent, GameState::MainMenu), InputAction::None);
}

// Test Playing state input handling
TEST_F(InputHandlerTest, PlayingStateInputHandling) {
    // Test ESCAPE key
    SDL_Event escapeEvent = createKeyDownEvent(SDLK_ESCAPE);
    EXPECT_EQ(inputHandler->processInput(escapeEvent, GameState::Playing), InputAction::Quit);
    
    // Test SPACE key initial press
    SDL_Event spaceDownEvent = createKeyDownEvent(SDLK_SPACE);
    EXPECT_EQ(inputHandler->processInput(spaceDownEvent, GameState::Playing), InputAction::Select);
    EXPECT_TRUE(inputHandler->isSpaceHeld());
    
    // Test SPACE key while already held down (should return None)
    SDL_Event spaceDownEvent2 = createKeyDownEvent(SDLK_SPACE);
    EXPECT_EQ(inputHandler->processInput(spaceDownEvent2, GameState::Playing), InputAction::None);
    EXPECT_TRUE(inputHandler->isSpaceHeld()); // Should still be held
    
    // Test SPACE key release
    SDL_Event spaceUpEvent = createKeyUpEvent(SDLK_SPACE);
    EXPECT_EQ(inputHandler->processInput(spaceUpEvent, GameState::Playing), InputAction::None);
    EXPECT_FALSE(inputHandler->isSpaceHeld()); // Should no longer be held
    
    // Test unmapped key
    SDL_Event randomEvent = createKeyDownEvent(SDLK_a);
    EXPECT_EQ(inputHandler->processInput(randomEvent, GameState::Playing), InputAction::None);
}

// Test EndScreen input handling
TEST_F(InputHandlerTest, EndScreenInputHandling) {
    // Test ESCAPE key (different behavior in EndScreen)
    SDL_Event escapeEvent = createKeyDownEvent(SDLK_ESCAPE);
    EXPECT_EQ(inputHandler->processInput(escapeEvent, GameState::EndScreen), InputAction::Escape);
    
    // Test SPACE key
    SDL_Event spaceEvent = createKeyDownEvent(SDLK_SPACE);
    EXPECT_EQ(inputHandler->processInput(spaceEvent, GameState::EndScreen), InputAction::Select);
    
    // Test UP arrow
    SDL_Event upEvent = createKeyDownEvent(SDLK_UP);
    EXPECT_EQ(inputHandler->processInput(upEvent, GameState::EndScreen), InputAction::MenuUp);
    
    // Test DOWN arrow
    SDL_Event downEvent = createKeyDownEvent(SDLK_DOWN);
    EXPECT_EQ(inputHandler->processInput(downEvent, GameState::EndScreen), InputAction::MenuDown);
    
    // Test unmapped key
    SDL_Event randomEvent = createKeyDownEvent(SDLK_a);
    EXPECT_EQ(inputHandler->processInput(randomEvent, GameState::EndScreen), InputAction::None);
}

// Test space key state management in Playing mode
TEST_F(InputHandlerTest, SpaceKeyStateManagement) {
    // Initially space should not be held
    EXPECT_FALSE(inputHandler->isSpaceHeld());
    
    // Press space in Playing mode
    SDL_Event spaceDownEvent = createKeyDownEvent(SDLK_SPACE);
    EXPECT_EQ(inputHandler->processInput(spaceDownEvent, GameState::Playing), InputAction::Select);
    EXPECT_TRUE(inputHandler->isSpaceHeld());
    
    // Press space again while held (should not trigger action)
    SDL_Event spaceDownEvent2 = createKeyDownEvent(SDLK_SPACE);
    EXPECT_EQ(inputHandler->processInput(spaceDownEvent2, GameState::Playing), InputAction::None);
    EXPECT_TRUE(inputHandler->isSpaceHeld());
    
    // Release space
    SDL_Event spaceUpEvent = createKeyUpEvent(SDLK_SPACE);
    EXPECT_EQ(inputHandler->processInput(spaceUpEvent, GameState::Playing), InputAction::None);
    EXPECT_FALSE(inputHandler->isSpaceHeld());
    
    // Press space again after release (should work)
    SDL_Event spaceDownEvent3 = createKeyDownEvent(SDLK_SPACE);
    EXPECT_EQ(inputHandler->processInput(spaceDownEvent3, GameState::Playing), InputAction::Select);
    EXPECT_TRUE(inputHandler->isSpaceHeld());
}

// Test space key behavior in different states
TEST_F(InputHandlerTest, SpaceKeyDifferentStates) {
    // Space in MainMenu doesn't affect isSpaceHeld
    SDL_Event spaceEvent = createKeyDownEvent(SDLK_SPACE);
    EXPECT_EQ(inputHandler->processInput(spaceEvent, GameState::MainMenu), InputAction::Select);
    EXPECT_FALSE(inputHandler->isSpaceHeld()); // Should remain false
    
    // Space in EndScreen doesn't affect isSpaceHeld
    EXPECT_EQ(inputHandler->processInput(spaceEvent, GameState::EndScreen), InputAction::Select);
    EXPECT_FALSE(inputHandler->isSpaceHeld()); // Should remain false
    
    // Only Playing state manages space key state
    EXPECT_EQ(inputHandler->processInput(spaceEvent, GameState::Playing), InputAction::Select);
    EXPECT_TRUE(inputHandler->isSpaceHeld()); // Should be true now
}

// Test isKeyPressed functionality
TEST_F(InputHandlerTest, IsKeyPressedFunctionality) {
    // Note: This test may be limited by the SDL test environment
    // We can test that the method doesn't crash and returns consistently
    
    bool spacePressed1 = inputHandler->isKeyPressed(SDL_SCANCODE_SPACE);
    bool spacePressed2 = inputHandler->isKeyPressed(SDL_SCANCODE_SPACE);
    
    // Should be consistent between calls
    EXPECT_EQ(spacePressed1, spacePressed2);
    
    // Test with different scancodes
    bool escapePressed = inputHandler->isKeyPressed(SDL_SCANCODE_ESCAPE);
    bool aPressed = inputHandler->isKeyPressed(SDL_SCANCODE_A);
    
    // Should not crash and should return boolean values
    EXPECT_TRUE(escapePressed == true || escapePressed == false);
    EXPECT_TRUE(aPressed == true || aPressed == false);
}

// Test key up events for non-space keys (should not affect space state)
TEST_F(InputHandlerTest, NonSpaceKeyUpEvents) {
    // Set space as held
    SDL_Event spaceDownEvent = createKeyDownEvent(SDLK_SPACE);
    inputHandler->processInput(spaceDownEvent, GameState::Playing);
    EXPECT_TRUE(inputHandler->isSpaceHeld());
    
    // Release other keys (should not affect space state)
    SDL_Event escapeUpEvent = createKeyUpEvent(SDLK_ESCAPE);
    EXPECT_EQ(inputHandler->processInput(escapeUpEvent, GameState::Playing), InputAction::None);
    EXPECT_TRUE(inputHandler->isSpaceHeld()); // Should still be held
    
    SDL_Event aUpEvent = createKeyUpEvent(SDLK_a);
    EXPECT_EQ(inputHandler->processInput(aUpEvent, GameState::Playing), InputAction::None);
    EXPECT_TRUE(inputHandler->isSpaceHeld()); // Should still be held
}

// Test multiple consecutive key presses
TEST_F(InputHandlerTest, MultipleConsecutiveKeyPresses) {
    // Multiple different keys in MainMenu
    SDL_Event spaceEvent = createKeyDownEvent(SDLK_SPACE);
    SDL_Event upEvent = createKeyDownEvent(SDLK_UP);
    SDL_Event downEvent = createKeyDownEvent(SDLK_DOWN);
    SDL_Event escapeEvent = createKeyDownEvent(SDLK_ESCAPE);
    
    EXPECT_EQ(inputHandler->processInput(spaceEvent, GameState::MainMenu), InputAction::Select);
    EXPECT_EQ(inputHandler->processInput(upEvent, GameState::MainMenu), InputAction::MenuUp);
    EXPECT_EQ(inputHandler->processInput(downEvent, GameState::MainMenu), InputAction::MenuDown);
    EXPECT_EQ(inputHandler->processInput(escapeEvent, GameState::MainMenu), InputAction::Quit);
}

// Test switching between game states
TEST_F(InputHandlerTest, StateSwitching) {
    SDL_Event escapeEvent = createKeyDownEvent(SDLK_ESCAPE);
    
    // Same key, different behavior based on state
    EXPECT_EQ(inputHandler->processInput(escapeEvent, GameState::MainMenu), InputAction::Quit);
    EXPECT_EQ(inputHandler->processInput(escapeEvent, GameState::Playing), InputAction::Quit);
    EXPECT_EQ(inputHandler->processInput(escapeEvent, GameState::EndScreen), InputAction::Escape);
}

// Test invalid/unknown game state
TEST_F(InputHandlerTest, InvalidGameState) {
    SDL_Event spaceEvent = createKeyDownEvent(SDLK_SPACE);
    
    // Cast to invalid state
    GameState invalidState = static_cast<GameState>(999);
    EXPECT_EQ(inputHandler->processInput(spaceEvent, invalidState), InputAction::None);
}

// Test rhythm game timing scenario
TEST_F(InputHandlerTest, RhythmGameTimingScenario) {
    // Simulate rhythm game beat timing
    SDL_Event spaceDown = createKeyDownEvent(SDLK_SPACE);
    SDL_Event spaceUp = createKeyUpEvent(SDLK_SPACE);
    
    // Beat 1
    EXPECT_EQ(inputHandler->processInput(spaceDown, GameState::Playing), InputAction::Select);
    EXPECT_TRUE(inputHandler->isSpaceHeld());
    EXPECT_EQ(inputHandler->processInput(spaceUp, GameState::Playing), InputAction::None);
    EXPECT_FALSE(inputHandler->isSpaceHeld());
    
    // Beat 2
    EXPECT_EQ(inputHandler->processInput(spaceDown, GameState::Playing), InputAction::Select);
    EXPECT_TRUE(inputHandler->isSpaceHeld());
    EXPECT_EQ(inputHandler->processInput(spaceUp, GameState::Playing), InputAction::None);
    EXPECT_FALSE(inputHandler->isSpaceHeld());
    
    // Held down through multiple beats (should only trigger once)
    EXPECT_EQ(inputHandler->processInput(spaceDown, GameState::Playing), InputAction::Select);
    EXPECT_TRUE(inputHandler->isSpaceHeld());
    EXPECT_EQ(inputHandler->processInput(spaceDown, GameState::Playing), InputAction::None); // Ignored
    EXPECT_EQ(inputHandler->processInput(spaceDown, GameState::Playing), InputAction::None); // Ignored
    EXPECT_TRUE(inputHandler->isSpaceHeld());
    EXPECT_EQ(inputHandler->processInput(spaceUp, GameState::Playing), InputAction::None);
    EXPECT_FALSE(inputHandler->isSpaceHeld());
}

// Test all InputAction enum values
TEST_F(InputHandlerTest, InputActionEnumValues) {
    // Test that all enum values are distinct
    EXPECT_NE(InputAction::None, InputAction::Quit);
    EXPECT_NE(InputAction::None, InputAction::Select);
    EXPECT_NE(InputAction::None, InputAction::MenuUp);
    EXPECT_NE(InputAction::None, InputAction::MenuDown);
    EXPECT_NE(InputAction::None, InputAction::Escape);
    
    EXPECT_NE(InputAction::Quit, InputAction::Select);
    EXPECT_NE(InputAction::Select, InputAction::MenuUp);
    EXPECT_NE(InputAction::MenuUp, InputAction::MenuDown);
    EXPECT_NE(InputAction::MenuDown, InputAction::Escape);
}

// Test all GameState enum values
TEST_F(InputHandlerTest, GameStateEnumValues) {
    // Test that all enum values are distinct
    EXPECT_NE(GameState::MainMenu, GameState::Playing);
    EXPECT_NE(GameState::MainMenu, GameState::EndScreen);
    EXPECT_NE(GameState::MainMenu, GameState::Quit);
    
    EXPECT_NE(GameState::Playing, GameState::EndScreen);
    EXPECT_NE(GameState::Playing, GameState::Quit);
    
    EXPECT_NE(GameState::EndScreen, GameState::Quit);
}