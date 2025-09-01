#include "ResourceManager.hpp"
#include "Logger.hpp"

#include <iostream>

ResourceManager::ResourceManager(SDL_Renderer* renderer) : renderer(renderer), m_valid(false) {
    if (renderer == nullptr) {
        Logger::error("ResourceManager: null renderer provided");
        return;
    }
    m_valid = true;
}

ResourceManager::~ResourceManager() {
    cleanup();
}

SDL_Texture* ResourceManager::loadTexture(const std::string& filePath) {
    if (!m_valid) {
        Logger::error("ResourceManager::loadTexture called on invalid ResourceManager");
        return nullptr;
    }
    
    if (filePath.empty()) {
        Logger::error("ResourceManager::loadTexture called with empty file path");
        return nullptr;
    }
    
    // Check if texture already loaded
    auto it = textures.find(filePath);
    if (it != textures.end()) {
        return it->second;
    }
    
    // Load new texture
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
    if (!texture) {
        Logger::logSDLImageError(LogLevel::ERROR, "Failed to load texture: " + filePath);
        return nullptr;
    }
    
    // Cache the texture
    textures[filePath] = texture;
    Logger::debug("Loaded texture: " + filePath);
    return texture;
}

SDL_Texture* ResourceManager::createTextTexture(const std::string& fontPath, int fontSize, const std::string& text, SDL_Color color) {
    if (!m_valid) {
        Logger::error("ResourceManager::createTextTexture called on invalid ResourceManager");
        return nullptr;
    }
    
    if (fontPath.empty()) {
        Logger::error("ResourceManager::createTextTexture called with empty font path");
        return nullptr;
    }
    
    if (text.empty()) {
        Logger::warning("ResourceManager::createTextTexture called with empty text");
        return nullptr;
    }
    
    std::string textKey = generateTextKey(fontPath, fontSize, text, color);
    
    // Check if text texture already exists
    auto it = textures.find(textKey);
    if (it != textures.end()) {
        return it->second;
    }
    
    // Get or load font
    Font* font = getFont(fontPath, fontSize);
    if (!font) {
        Logger::error("Failed to get font for text texture: " + fontPath);
        return nullptr;
    }
    
    // Create text texture
    SDL_Texture* textTexture = font->renderText(renderer, text, color);
    if (textTexture) {
        textures[textKey] = textTexture;
        Logger::debug("Created text texture: " + text);
    } else {
        Logger::error("Failed to create text texture for: " + text);
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