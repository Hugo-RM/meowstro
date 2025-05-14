#include "Sprite.hpp"

Sprite::Sprite(float x, float y, SDL_Texture* texture, int row, int col) : Entity(x, y, texture), row(row), col(col)
{
	int textureW, textureH;
	SDL_QueryTexture(texture, NULL, NULL, &textureW, &textureH);

	frameWidth = textureW / col;
	frameHeight = textureH / row;

	currentFrame.w = frameWidth;
	currentFrame.h = frameHeight;
}
void Sprite::setFrame(int row, int col)
{
	if (row > 0)
		this->row = row;
	else
		this->row = 1;
	if (col > 0)
		this->col = col;
	else
		this->col = 1;
}
void Sprite::setLoc(int x, int y)
{
	setX(x);
	setY(y);
}
void Sprite::moveUp(int y)
{
	if (y > 0)
		setY(getY() + y);
}
void Sprite::moveDown(int y)
{
	if (y < 0)
		setY(getY() - y);
}
void Sprite::moveRight(int x)
{
	if (x > 0)
		setX(getX() + x);
}
void Sprite::moveLeft(int x)
{
	if (x < 0)
		setX(getX() - x);
}
