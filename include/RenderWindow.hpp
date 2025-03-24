#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Entity.hpp"

class RenderWindow
{
public:
	RenderWindow(const char *title, int w, int h, Uint32 windowFlags = SDL_WINDOW_SHOWN);
	SDL_Texture *loadTexture(const char *filePath);
	void clear();
	void render(Entity& entity);
	void display();
	~RenderWindow();
private:
	SDL_Window *window;
	SDL_Renderer *renderer;

};

