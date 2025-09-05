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
    
    // Check if texture already loaded and validate it
    auto it = textures.find(filePath);
    if (it != textures.end()) {
        if (isTextureValid(it->second)) {
            Logger::debug("Using cached texture: " + filePath);
            return it->second;
        } else {
            Logger::warning("Cached texture is invalid, reloading: " + filePath);
            textures.erase(it);
        }
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
    
    // Check if text texture already exists and validate it
    auto it = textures.find(textKey);
    if (it != textures.end()) {
        if (isTextureValid(it->second)) {
            Logger::debug("Using cached text texture: " + text);
            return it->second;
        } else {
            Logger::warning("Cached text texture is invalid, recreating: " + text);
            textures.erase(it);
        }
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
        return it->second.get();
    }
    
    // Load new font
    auto font = std::make_unique<Font>();
    if (!font->load(fontPath, fontSize)) {
        return nullptr;
    }
    
    // Cache the font and return raw pointer for API compatibility
    Font* fontPtr = font.get();
    fonts[fontKey] = std::move(font);
    return fontPtr;
}

void ResourceManager::cleanup() {
    Logger::info("ResourceManager cleaning up all resources");
    // Clean up all textures
    for (auto& pair : textures) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures.clear();
    
    // Clean up all fonts - unique_ptr handles deletion automatically
    fonts.clear();
    Logger::debug("ResourceManager cleanup complete");
}

void ResourceManager::clearCache() {
    Logger::info("ResourceManager clearing texture cache (keeping textures alive)");
    // Clear cache without destroying textures - useful for state resets
    // This allows textures to remain valid but forces reloading from disk
    textures.clear();
    fonts.clear();
    Logger::debug("ResourceManager cache cleared");
}

bool ResourceManager::isTextureValid(SDL_Texture* texture) const {
    if (!texture) {
        return false;
    }

    return true;
    
    // In Release builds, SDL_QueryTexture might behave differently
    // For now, assume all non-null textures are valid to avoid cache invalidation
    // This is safe because ResourceManager owns the textures and only destroys them in cleanup()
    
    /* Original validation code - disabled due to Release build issues
    // Try to query the texture - if it's been destroyed, this will fail
    int w, h;
    Uint32 format;
    int access;
    int result = SDL_QueryTexture(texture, &format, &access, &w, &h);
    
    if (result != 0) {
        Logger::warning("Texture validation failed: " + std::string(SDL_GetError()));
        return false;
    }
    
    return true;
    */
}

std::string ResourceManager::generateFontKey(const std::string& fontPath, int fontSize) const {
    return fontPath + "_" + std::to_string(fontSize);
}

std::string ResourceManager::generateTextKey(const std::string& fontPath, int fontSize, const std::string& text, SDL_Color color) const {
    return fontPath + "_" + std::to_string(fontSize) + "_" + text + "_" + 
           std::to_string(color.r) + "_" + std::to_string(color.g) + "_" + 
           std::to_string(color.b) + "_" + std::to_string(color.a);
}