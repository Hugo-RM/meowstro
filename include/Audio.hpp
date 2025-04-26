#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>

class Audio {
private:
    std::string myaudio_name;
public:
    Audio();
    Audio(std::string myaudio_name);
    int AudioPlayer(std::string path_of_audio);
};
