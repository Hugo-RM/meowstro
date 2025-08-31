#include <iostream>
#include "RenderWindow.hpp"


RenderWindow::RenderWindow(const char *title, int w, int h, Uint32 windowFlags) : window(NULL), renderer(NULL)
{
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, windowFlags);
	if (window == NULL)
	{
		std::cout << "Window failed to init. ERROR: " << SDL_GetError() << std::endl;
		return;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 100, 115, 180, 185);
}
void RenderWindow::clear()
{
	SDL_RenderClear(renderer);
}
void RenderWindow::render(Entity& entity)
{
	SDL_Rect src = entity.getCurrentFrame();
	SDL_Rect destination;

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