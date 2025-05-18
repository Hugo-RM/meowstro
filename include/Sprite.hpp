#pragma once
#include "Entity.hpp"

class Sprite : public Entity
{
public:
	Sprite(float x, float y, SDL_Texture* texture, int maxRow, int maxCol);
	
	void setFrame(int row, int col);
	inline void resetFrame()
	{
		setFrame(1, 1);
	}
	
	inline int getRow() const
	{
		return row;
	}
	inline int getCol() const
	{
		return col;
	}

	inline void setLoc(int x, int y)
	{
		setX(x);
		setY(y);
	}
	inline void moveUp(int dy)
	{
		setY(getY() - dy);
	}
	inline void moveDown(int dy)
	{
		setY(getY() + dy);
	}
	inline void moveRight(int dx)
	{
		setX(getX() + dx);
	}
	inline void moveLeft(int dx)
	{
		setX(getX() - dx);
	}
	
	Sprite operator ++(int);
private:
	int row;
	int col;
	int maxRow;
	int maxCol;
	int frameWidth;
	int frameHeight;
};