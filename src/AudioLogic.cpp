#include "AudioLogic.hpp"

//ExpectedHit AudioLogic::
AudioLogic::AudioLogic() {

}
AudioLogic::~AudioLogic() {

}
void AudioLogic::converteToBST(int ms, int bpm, ExpectedHit& hit) {
	double beatsPerMs = bpm / 60000.0;
	hit.beat = ms * beatsPerMs;
	hit.second = (ms / 10000) % 60;
	hit.tick = fmod(hit.beat, 1.0);

}
void AudioLogic::checkHit(ExpectedHit target, ExpectedHit user, int bpm) {
	int expectedMs = convertBSTtoMs(target, bpm);
	int actualMs = convertBSTtoMs(user, bpm);
	int delta = abs(expectedMs - actualMs);

	if (delta <= PERFECT_WINDOW_MS) {
		std::cout << "Perfect" << std::endl;
	}
	else if (delta <= GOOD_WINDOW_MS) {
		std::cout << "Good" << std::endl;
	}
	else {
		std::cout << "Miss" << std::endl;
	}
}
ExpectedHit AudioLogic::getHit(int index) {
	return myNotes[index];
}
void AudioLogic::updateTime(int currentMs, int bpm) {
	ExpectedHit gameTime;
	gameTime = { 0,0,0.0 };
	converteToBST(currentMs, 147, gameTime);
}
int AudioLogic::convertBSTtoMs(const ExpectedHit& hit, int bpm) {
	double msPerBeat = 60000.0 / bpm; // One beat duration in ms

	int beatMs = hit.beat * msPerBeat;  // Convert beats to milliseconds
	int secondMs = hit.second * 1000;   // Convert seconds to milliseconds
	int tickMs = hit.tick * msPerBeat;  // Convert fractional ticks to milliseconds

	return static_cast<int>(beatMs + secondMs + tickMs);
}
int AudioLogic::getPERFECTW() const {
	return PERFECT_WINDOW_MS;
}
int AudioLogic::getGOODW() const {
	return GOOD_WINDOW_MS;
}
