#pragma once
#include "Entity.hpp"

class Sprite : public Entity
{
public:
	Sprite(float x, float y, SDL_Texture* texture, int row, int col);
	void setFrame(int row, int col);
	void setLoc(int x, int y);
	void moveUp(int y);
	void moveDown(int y);
	void moveRight(int x);
	void moveLeft(int x);
private:
	int row;
	int col;
	int frameWidth;
	int frameHeight;
};