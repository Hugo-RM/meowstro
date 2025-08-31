/*
Author: Leonardo Lopez
Modified: 5/14/25
Description: Font class, handles the logic when rendering a true type font text into an SDL texture
*/
#ifndef FONT_H
#define FONT_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

using namespace std;

class Font 
{
    public:
        Font(); // Basic Constructor 
        ~Font(); // Destructor
        bool load(const string& fontPath, int fontSz); // Allows us to add a font file
        void unload(); // Clean up (or unload) the font

        // This allows us to make the switch from text (ttf or any font file) to texture (SDL)
        SDL_Texture* renderText(SDL_Renderer* renderer, const string& txt, SDL_Color color); 
    private: 
    TTF_Font* font; // Internal pointer, which points to the loaded font 
};

#endif
