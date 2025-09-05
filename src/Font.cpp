#include "Font.hpp"
#include <iostream>

Font::Font() : font(nullptr) {}
       
Font::~Font()
{
    unload();
}

bool Font::load(const std::string& fontPath, int fontSz)
{
    font = TTF_OpenFont(fontPath.c_str(), fontSz); // Opens up a font file, if works: true if !works: false
    if (!font)
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
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

SDL_Texture* Font::renderText(SDL_Renderer* renderer, const std::string& txt, SDL_Color color)
{
    if (!font)
    {
        return nullptr;
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(font, txt.c_str(), color);
    if (!textSurface)
    {
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface); // Surface freed after conversion to texture

    return texture; // It's on the caller's part to destroy the texture 
}
