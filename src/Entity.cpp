#include "Entity.hpp"

// Constructor overload for raw SDL_Texture* (backward compatibility)
Entity::Entity(float x, float y, SDL_Texture* texture) : Entity(x, y, texture ? makeSharedSDLTexture(texture) : nullptr) {
}

Entity::Entity(float x, float y, SharedSDLTexture texture) : x(x), y(y), texture_(texture)
{
	currentFrame.x = 0;
	currentFrame.y = 0;
	// Automatically detect texture size with error checking
	int textureW, textureH;
	if (texture_ && texture_->getSize(textureW, textureH)) {
		currentFrame.w = textureW;
		currentFrame.h = textureH;
	} else {
		// Default size for null or invalid textures
		currentFrame.w = 0;
		currentFrame.h = 0;
	}
}
void Entity::setCurrentFrameW(int w)
{
	currentFrame.w = w;
}
void Entity::setCurrentFrameH(int h)
{
	currentFrame.h = h;
}
SDL_Rect Entity::getCurrentFrame()
{
	return currentFrame;
}