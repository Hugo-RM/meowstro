#pragma once
#include <SDL.h>
#include <SDL_image.h>

class Entity
{
public:
	Entity(float x, float y, SDL_Texture* texture);
	inline float getX() const
	{
		return x;
	}
	inline float getY() const
	{
		return y;
	}
	inline SDL_Texture *getTexture()
	{
		return texture;
	}
	SDL_Rect getCurrentFrame();
	void setCurrentFrameW(int w);
	void setCurrentFrameH(int h);
	inline void setTexture(SDL_Texture* texture)
	{
		this->texture = texture;
	}
protected:
	inline void setX(int x)
	{
		this->x = x;
	}
	inline void setY(int y)
	{
		this->y = y;
	}
	float x;
	float y;
	SDL_Rect currentFrame;
	SDL_Texture* texture;
};
