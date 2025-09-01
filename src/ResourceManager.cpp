#include "ResourceManager.hpp"

#include <iostream>

ResourceManager::ResourceManager(SDL_Renderer* renderer) : renderer(renderer) {}

ResourceManager::~ResourceManager() {
    cleanup();
}

SDL_Texture* ResourceManager::loadTexture(const std::string& filePath) {
    // Check if texture already loaded
    auto it = textures.find(filePath);
    if (it != textures.end()) {
        return it->second;
    }
    
    // Load new texture
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture: " << filePath << " SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    
    // Cache the texture
    textures[filePath] = texture;
    return texture;
}

SDL_Texture* ResourceManager::createTextTexture(const std::string& fontPath, int fontSize, const std::string& text, SDL_Color color) {
    std::string textKey = generateTextKey(fontPath, fontSize, text, color);
    
    // Check if text texture already exists
    auto it = textures.find(textKey);
    if (it != textures.end()) {
        return it->second;
    }
    
    // Get or load font
    Font* font = getFont(fontPath, fontSize);
    if (!font) {
        return nullptr;
    }
    
    // Create text texture
    SDL_Texture* textTexture = font->renderText(renderer, text, color);
    if (textTexture) {
        textures[textKey] = textTexture;
    }
    
    return textTexture;
}

Font* ResourceManager::getFont(const std::string& fontPath, int fontSize) {
    std::string fontKey = generateFontKey(fontPath, fontSize);
    
    // Check if font already loaded
    auto it = fonts.find(fontKey);
    if (it != fonts.end()) {
        return it->second;
    }
    
    // Load new font
    Font* font = new Font();
    if (!font->load(fontPath, fontSize)) {
        delete font;
        return nullptr;
    }
    
    // Cache the font
    fonts[fontKey] = font;
    return font;
}

void ResourceManager::cleanup() {
    // Clean up all textures
    for (auto& pair : textures) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures.clear();
    
    // Clean up all fonts
    for (auto& pair : fonts) {
        if (pair.second) {
            delete pair.second;
        }
    }
    fonts.clear();
}

std::string ResourceManager::generateFontKey(const std::string& fontPath, int fontSize) const {
    return fontPath + "_" + std::to_string(fontSize);
}

std::string ResourceManager::generateTextKey(const std::string& fontPath, int fontSize, const std::string& text, SDL_Color color) const {
    return fontPath + "_" + std::to_string(fontSize) + "_" + text + "_" + 
           std::to_string(color.r) + "_" + std::to_string(color.g) + "_" + 
           std::to_string(color.b) + "_" + std::to_string(color.a);
}