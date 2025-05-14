#pragma once
#include <iostream>
#include <SDL.h>

class Level {
public:
	void checkTiming(Uint32 keyPressTime, Uint32 currentTime);
	Uint32 getCurrentBeat(Uint32 currentTime);
	Level();
	Level(int BMP_of_song, int milisecsPerBeat);
private:
	Uint32 startTime;
	const float perferctScore = 50;
	const float greatScore = 100;
	int BMP_of_song;
	int milisecsPerBeat;
};
