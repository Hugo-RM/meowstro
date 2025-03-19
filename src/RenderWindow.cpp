#include <iostream>
#include "RenderWindow.hpp"


RenderWindow::RenderWindow(const char* title, int w, int h) : window(NULL), renderer(NULL)
{
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		std::cout << "Window failed to init. ERROR: " << SDL_GetError() << std::endl;
		return;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

SDL_Texture* RenderWindow::loadTexture(const char* filePath)
{
	SDL_Texture* texture = NULL;
	texture = IMG_LoadTexture(renderer, filePath);

	if (texture == NULL)
		std::cout << "IMG_LoadTexture failed. ERROR: " << SDL_GetError() << std::endl;

	return texture;
}

void RenderWindow::clear()
{
	SDL_RenderClear(renderer);
}
void RenderWindow::render(Entity& entity)
{
	SDL_Rect src;
	SDL_Rect destination;

	src.x = entity.getCurrentFrame().x;
	src.y = entity.getCurrentFrame().y;
	src.w = entity.getCurrentFrame().w;
	src.h = entity.getCurrentFrame().h;

	destination.x = entity.getX();
	destination.y = entity.getY();
	destination.w = entity.getCurrentFrame().w;
	destination.h = entity.getCurrentFrame().h;


	SDL_RenderCopy(renderer, entity.getTexture(), &src, &destination);
}
void RenderWindow::display()
{
	SDL_RenderPresent(renderer);
}

RenderWindow::~RenderWindow()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}