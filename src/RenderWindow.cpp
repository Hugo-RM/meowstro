#include <iostream>
#include "RenderWindow.hpp"
#include "Logger.hpp"


RenderWindow::RenderWindow(const char *title, int w, int h, Uint32 windowFlags) 
    : window(nullptr), renderer(nullptr), m_valid(false)
{
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, windowFlags);
	if (window == nullptr)
	{
		Logger::logSDLError(LogLevel::ERROR, "Failed to create window");
		return;
	}
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		Logger::logSDLError(LogLevel::ERROR, "Failed to create renderer");
		SDL_DestroyWindow(window);
		window = nullptr;
		return;
	}
	
	SDL_SetRenderDrawColor(renderer, 100, 115, 180, 185);
	m_valid = true;
}
void RenderWindow::clear()
{
	if (m_valid && renderer) {
		SDL_RenderClear(renderer);
	}
}
void RenderWindow::render(Entity& entity)
{
	if (!m_valid || !renderer) {
		Logger::error("RenderWindow::render called on invalid window");
		return;
	}
	
	if (entity.getTexture() == nullptr) {
		Logger::warning("RenderWindow::render called with null texture");
		return;
	}
	
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
	if (m_valid && renderer) {
		SDL_RenderPresent(renderer);
	}
}

RenderWindow::~RenderWindow()
{
	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}
	if (window) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}