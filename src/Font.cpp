#include "Font.hpp"
#include <iostream>

using namespace std;

Font::Font() : font(nullptr) {} // Init ptr 2 nullptr
       
Font::~Font()
{
    unload(); // Makin sure memory is nice n tidy
}

bool Font::load(const string& fontPath, int fontSz)
{
    font = TTF_OpenFont(fontPath.c_str(), fontSz); // Opens up a font file, if works: true if !works: false
    if (!font)
    {
        cerr << "Failed to load font: " << TTF_GetError() << endl;
        return false;
    }
    return true;
}

void Font::unload()
{
    if (font)
    {
        TTF_CloseFont(font); // Setting free the font resource
        font = nullptr; // No messy ptrs
    }
}

SDL_Texture* Font::renderText(SDL_Renderer* renderer, const string& txt, SDL_Color color)
{
    if (!font)
    {
        return nullptr;
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!textSurface)
    {
        cerr << "Failed to create text surface: " << TTF_GetError() << endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface); // Surface freed after conversion to texture

    return texture; // It's on the caller's part to destroy the texture 
}
