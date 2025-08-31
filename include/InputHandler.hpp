#pragma once

#include <SDL.h>

enum class InputAction {
    None,
    Quit,
    Select,         // SPACE key
    MenuUp,         // UP arrow (menu navigation)
    MenuDown,       // DOWN arrow (menu navigation)
    Escape          // ESC key
};

enum class GameState {
    MainMenu,
    Playing,
    EndScreen
};

class InputHandler
{
public:
    InputHandler();
    ~InputHandler() = default;
    
    // Process SDL events and return the appropriate action
    InputAction processInput(SDL_Event& event, GameState currentState);
    
    // Check if a key is currently pressed (for game state)
    bool isKeyPressed(SDL_Scancode key) const;
    
    // Check if space key is being held down (for rhythm game timing)
    bool isSpaceHeld() const;
    
private:
    const Uint8* keyboardState;
    bool spaceKeyDown; // Track space key state for rhythm timing
    
    // Helper methods for different game states
    InputAction processMenuInput(const SDL_Event& event);
    InputAction processGameInput(const SDL_Event& event);
    InputAction processEndScreenInput(const SDL_Event& event);
};