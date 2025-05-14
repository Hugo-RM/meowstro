#pragma once
#include <SDL.h>
#include <SDL_image.h>

class Entity
{
public:
	Entity(float x, float y, SDL_Texture* texture);
	float getX() const;
	float getY() const;
	SDL_Texture *getTexture();
	SDL_Rect getCurrentFrame();
	void setCurrentFrameW(int w);
	void setCurrentFrameH(int h);
protected:
	void setX(int x);
	void setY(int y);
	float x;
	float y;
	SDL_Rect currentFrame;
	SDL_Texture* texture;
};
