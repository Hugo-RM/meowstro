#include "Level.hpp"

Level::Level() {
	startTime = SDL_GetTicks();
	BMP_of_song = 120;
	milisecsPerBeat = 60000;

}
Level::Level(int BMP_of_song, int milisecsPerBeat) {
	this->BMP_of_song = BMP_of_song;
	this->milisecsPerBeat = milisecsPerBeat;
}
void Level::checkTiming(Uint32 keyPressTime, Uint32 currentTime) {
	Uint32 currentBeat = getCurrentBeat(currentTime);
	Uint32 expectedTime = startTime + currentBeat * milisecsPerBeat;
	int delta = keyPressTime - expectedTime;

	if (abs(delta) <= perferctScore) {
		std::cout << "Perfect!" << std::endl;
	}
	else if (abs(delta) <= greatScore) {
		std::cout << "Good!" << std::endl;
	}
	else {
		std::cout << "Miss!" << std::endl;
	}
}
Uint32 Level::getCurrentBeat(Uint32 currentTime) {
	return (currentTime - startTime) / milisecsPerBeat;
}
