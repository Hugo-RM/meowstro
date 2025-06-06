#include "Entity.hpp"

Entity::Entity(float x, float y, SDL_Texture* texture) : x(x), y(y), texture(texture)
{
	currentFrame.x = 0;
	currentFrame.y = 0;
	// Automatically detect texture size
	int textureW, textureH;
	SDL_QueryTexture(texture, NULL, NULL, &textureW, &textureH);
	currentFrame.w = textureW;
	currentFrame.h = textureH;
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