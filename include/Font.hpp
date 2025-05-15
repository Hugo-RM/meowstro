/*
Author: Leonardo Lopez
Modified: 5/14/25
Description: Font class, handles the logic when rendering a true type font text into an SDL texture
*/
#ifndef FONT_H
#define FONT_H
/*
It is very important to note that I didn't download the necessary library (ttf) in order to work.
Nor have I downloaded the font that we decided to use for the project, that being Comic Sans.
As I was literally passing out while writing this because it was so late and rarely have time.
This also explains why there is no driver file to test, again, passing out, so hypothetically this should work.
*/
#include <SDL.h> // Not sure if this needs to be .hpp, again, verge of passing out
#include <SDL_ttf.h>
#include <string>

using namespace std;

class Font 
{
    public:
        Font(); // Basic Constructor 
        ~Font(); // Destructor (fancy)
        bool load(const string& fontPath, int fontSz); // Allows us to add a font file
        void unload(); // Clean up (or unload) the font

        // This allows us to make the switch from text (ttf or any font file) to texture (SDL)
        SDL_Texture* renderText(SDL_Renderer* renderer, const string& txt, SDL_Color color); 
    private: 
    TTF_Font* font; // Internal pointer, which points to the loaded font 
};

#endif
