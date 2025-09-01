#pragma once
#include <SDL.h>
#include "Entity.hpp"

class RenderWindow
{
public:
	RenderWindow(const char *title, int w, int h, Uint32 windowFlags = SDL_WINDOW_SHOWN);
	void clear();
	void render(Entity& entity);
	void display();
	~RenderWindow();

	inline SDL_Renderer* getRenderer() const
	{
		return renderer;
	}
	
	bool isValid() const { return m_valid; }
	
private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	bool m_valid;

};

