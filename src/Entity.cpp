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
void Entity::setCurrentFrameW(int w)
{
	currentFrame.w = w;
}
void Entity::setCurrentFrameH(int h)
{
	currentFrame.h = h;
}
void Entity::setX(int x)
{
	if (x < 0)
		this->x = 0;
	else if (x > 1920)
		this->x = 1920;
	else
		this->x = x;
}

void Entity::setY(int y)
{
	if (y < 0)
		this->y = 0;
	else if (y > 1080)
		this->y = 1080;
	else
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