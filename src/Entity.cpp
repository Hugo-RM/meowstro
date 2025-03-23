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
float Entity::getX()
{
	return x;
}
float Entity::getY()
{
	return y;
}
SDL_Texture *Entity::getTexture()
{
	return texture;
}
SDL_Rect Entity::getCurrentFrame()
{
	return currentFrame;
}