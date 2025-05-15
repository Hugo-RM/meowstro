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
protected:
	inline void setX(int x)
	{
		if (x < 0)
			this->x = 0;
		else if (x > 1920)
			this->x = 1920;
		else
			this->x = x;
	}
	inline void setY(int y)
	{
		if (y < 0)
			this->y = 0;
		else if (y > 1080)
			this->y = 1080;
		else
			this->y = y;
	}
	float x;
	float y;
	SDL_Rect currentFrame;
	SDL_Texture* texture;
};
