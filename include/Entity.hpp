#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "SDLTexture.hpp"
#include <memory>

class Entity
{
public:
	// Constructor that takes shared ownership of texture
	Entity(float x, float y, SharedSDLTexture texture);
	// Constructor for raw SDL_Texture* (creates shared ownership)
	Entity(float x, float y, SDL_Texture* texture);
	inline float getX() const
	{
		return x;
	}
	inline float getY() const
	{
		return y;
	}
	// Get raw texture pointer for SDL API calls
	inline SDL_Texture* getTexture() const
	{
		// Return shared texture if available, otherwise raw texture
		return texture_ ? texture_->get() : rawTexture_;
	}
	// Get shared texture for ownership transfer
	inline SharedSDLTexture getSharedTexture() const
	{
		return texture_;
	}
	SDL_Rect getCurrentFrame();
	void setCurrentFrameW(int w);
	void setCurrentFrameH(int h);
	// Set texture with shared ownership
	inline void setTexture(SharedSDLTexture texture)
	{
		texture_ = texture;
	}
	// Set texture from raw pointer (non-owning reference)
	inline void setTexture(SDL_Texture* texture)
	{
		rawTexture_ = texture;
		texture_ = nullptr;  // Clear shared texture
	}
protected:
	// Fixed type consistency - use float to match member variables
	inline void setX(float x)
	{
		this->x = x;
	}
	inline void setY(float y)
	{
		this->y = y;
	}
	float x;
	float y;
	SDL_Rect currentFrame;
	SharedSDLTexture texture_;       // For owned textures
	SDL_Texture* rawTexture_;       // For non-owned textures (ResourceManager-owned)
};
