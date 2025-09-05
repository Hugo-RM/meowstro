#include "InputHandler.hpp"

InputHandler::InputHandler()
    : spaceKeyDown(false)
{
    keyboardState = SDL_GetKeyboardState(NULL);
}

InputAction InputHandler::processInput(SDL_Event& event, GameState currentState)
{
    // Handle SDL_QUIT universally
    if (event.type == SDL_QUIT) {
        return InputAction::Quit;
    }
    
    // Delegate to appropriate state handler
    switch (currentState) {
        case GameState::MainMenu:
            return processMenuInput(event);
        case GameState::Playing:
            return processGameInput(event);
        case GameState::EndScreen:
            return processEndScreenInput(event);
        default:
            return InputAction::None;
    }
}

bool InputHandler::isKeyPressed(SDL_Scancode key) const
{
    return keyboardState[key] != 0;
}

bool InputHandler::isSpaceHeld() const
{
    return spaceKeyDown;
}

InputAction InputHandler::processMenuInput(const SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                return InputAction::Quit;
            case SDLK_SPACE:
                return InputAction::Select;
            case SDLK_UP:
                return InputAction::MenuUp;
            case SDLK_DOWN:
                return InputAction::MenuDown;
            default:
                break;
        }
    }
    return InputAction::None;
}

InputAction InputHandler::processGameInput(const SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                return InputAction::Quit;
            case SDLK_SPACE:
                // Only process spacebar on initial press (not when held)
                if (!spaceKeyDown) {
                    spaceKeyDown = true;
                    return InputAction::Select;
                }
                break; // Ignore repeated key events from holding
            default:
                break;
        }
    }
    else if (event.type == SDL_KEYUP) {
        if (event.key.keysym.sym == SDLK_SPACE) {
            spaceKeyDown = false; // Reset when space is released
        }
    }
    return InputAction::None;
}

InputAction InputHandler::processEndScreenInput(const SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                return InputAction::Escape;
            case SDLK_SPACE:
                return InputAction::Select;
            case SDLK_UP:
                return InputAction::MenuUp;
            case SDLK_DOWN:
                return InputAction::MenuDown;
            default:
                break;
        }
    }
    return InputAction::None;
}