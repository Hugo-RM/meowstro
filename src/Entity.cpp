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
float Entity::getX() const
{
	return x;
}
float Entity::getY() const
{
	return y;
}
void Entity::setX(int x) // needs right border
{
	if (x < 0) this->x = 0;
	this->x = x;
}
void Entity::setY(int x)
{
	if (y < 0) this->y = 0;
	this->y = y;
}
SDL_Texture *Entity::getTexture()
{
	return texture;
}
SDL_Rect Entity::getCurrentFrame()
{
	return currentFrame;
}