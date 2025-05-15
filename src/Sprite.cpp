#include "Sprite.hpp"

Sprite::Sprite(float x, float y, SDL_Texture* texture, int maxRow, int maxCol) : Entity(x, y, texture), maxRow(maxRow), maxCol(maxCol), row(1), col(1)
{
	int textureW, textureH;
	SDL_QueryTexture(texture, NULL, NULL, &textureW, &textureH);

	frameWidth = textureW / maxCol;
	frameHeight = textureH / maxRow;

	currentFrame.w = frameWidth;
	currentFrame.h = frameHeight;
}
void Sprite::setFrame(int row, int col)
{
	if (row >= 1 && row <= maxRow && col >= 1 && col <= maxCol)
	{
		this->row = row;
		this->col = col;
		currentFrame.x = (this->col - 1) * frameWidth;
		currentFrame.y = (this->row - 1) * frameHeight;
	}
}
Sprite Sprite::operator++(int)
{
	Sprite temp = *this; // copy current state

	row++;
	if (row > maxRow)
	{
		row = 1;
		col++;

		if (col > maxCol)
		{
			col = 1;
		}
	}

	setFrame(row, col);
	return temp;
}
