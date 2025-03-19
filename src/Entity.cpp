#include "Entity.hpp"

Entity::Entity(float x, float y, SDL_Texture* texture) : x(x), y(y), texture(texture)
{
	currentFrame.x = 0;
	currentFrame.y = 0;
	currentFrame.w = 32;
	currentFrame.h = 32;
}
float Entity::getX()
{
	return x;
}
float Entity::getY()
{
	return y;
}
SDL_Texture* Entity::getTexture()
{
	return texture;
}
SDL_Rect Entity::getCurrentFrame()
{
	return currentFrame;
}