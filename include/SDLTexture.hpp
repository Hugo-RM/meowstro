#pragma once
#include <SDL.h>
#include <memory>

// RAII wrapper for SDL_Texture with proper ownership semantics
class SDLTexture {
public:
    // Constructor that takes ownership of an SDL_Texture
    explicit SDLTexture(SDL_Texture* texture = nullptr) : texture_(texture) {}
    
    // Destructor automatically cleans up the texture
    ~SDLTexture() {
        if (texture_) {
            SDL_DestroyTexture(texture_);
        }
    }
    
    // Move constructor
    SDLTexture(SDLTexture&& other) noexcept : texture_(other.texture_) {
        other.texture_ = nullptr;
    }
    
    // Move assignment operator
    SDLTexture& operator=(SDLTexture&& other) noexcept {
        if (this != &other) {
            if (texture_) {
                SDL_DestroyTexture(texture_);
            }
            texture_ = other.texture_;
            other.texture_ = nullptr;
        }
        return *this;
    }
    
    // Disable copy constructor and copy assignment (non-copyable resource)
    SDLTexture(const SDLTexture&) = delete;
    SDLTexture& operator=(const SDLTexture&) = delete;
    
    // Get raw pointer for SDL API calls
    SDL_Texture* get() const noexcept { return texture_; }
    
    // Release ownership and return raw pointer
    SDL_Texture* release() noexcept {
        SDL_Texture* temp = texture_;
        texture_ = nullptr;
        return temp;
    }
    
    // Reset with new texture (destroys current texture if it exists)
    void reset(SDL_Texture* texture = nullptr) {
        if (texture_) {
            SDL_DestroyTexture(texture_);
        }
        texture_ = texture;
    }
    
    // Check if texture is valid
    explicit operator bool() const noexcept { return texture_ != nullptr; }
    
    // Get texture dimensions safely
    bool getSize(int& width, int& height) const {
        if (!texture_) {
            width = height = 0;
            return false;
        }
        return SDL_QueryTexture(texture_, nullptr, nullptr, &width, &height) == 0;
    }

private:
    SDL_Texture* texture_;
};

// Type alias for shared ownership scenarios
using SharedSDLTexture = std::shared_ptr<SDLTexture>;

// Helper function to create shared SDL texture
inline SharedSDLTexture makeSharedSDLTexture(SDL_Texture* texture) {
    return std::make_shared<SDLTexture>(texture);
}