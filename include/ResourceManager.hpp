#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <unordered_map>
#include <string>
#include <memory>
#include "Font.hpp"

class ResourceManager {
public:
    ResourceManager(SDL_Renderer* renderer);
    ~ResourceManager();

    // Texture management
    SDL_Texture* loadTexture(const std::string& filePath);
    SDL_Texture* createTextTexture(const std::string& fontPath, int fontSize, const std::string& text, SDL_Color color);
    
    // Font management
    Font* getFont(const std::string& fontPath, int fontSize);
    
    // Manual cleanup (called automatically in destructor)
    void cleanup();
    
    // Validity checking
    bool isValid() const { return m_valid; }

private:
    SDL_Renderer* renderer;
    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, std::unique_ptr<Font>> fonts;
    bool m_valid;
    
    // Helper to generate unique keys
    std::string generateFontKey(const std::string& fontPath, int fontSize) const;
    std::string generateTextKey(const std::string& fontPath, int fontSize, const std::string& text, SDL_Color color) const;
};