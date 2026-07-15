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

	// All game/UI code positions entities in a fixed w x h coordinate space.
	// Locking the renderer's logical size to that same space means resizing
	// the window or entering fullscreen scales everything (with letterboxing
	// to preserve aspect ratio) instead of clipping or misaligning content.
	SDL_RenderSetLogicalSize(renderer, w, h);

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

bool RenderWindow::isFullscreen() const
{
	if (!window) {
		return false;
	}
	return (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
}

void RenderWindow::toggleFullscreen()
{
	if (!window) {
		return;
	}

	Uint32 flag = isFullscreen() ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP;
	if (SDL_SetWindowFullscreen(window, flag) != 0) {
		Logger::logSDLError(LogLevel::ERROR, "Failed to toggle fullscreen");
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