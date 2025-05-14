#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include <array>
#include <SDL.h>
#include <SDL_mixer.h>

struct ExpectedHit
{
	int beat;
	int second;
	double tick;
};
class AudioLogic
{
private:
	const int PERFECT_WINDOW_MS = 50;
	const int GOOD_WINDOW_MS = 1000;
	const double gap = 0.60;

	ExpectedHit myNotes[49] = {
		ExpectedHit{3, 2, gap}, ExpectedHit{5, 12, gap}, ExpectedHit{6, 13, gap},
		ExpectedHit{7, 8, gap}, ExpectedHit{8, 8, gap}, ExpectedHit{9, 0, gap},
		ExpectedHit{9, 8, gap}, ExpectedHit{10, 6, gap}, ExpectedHit{12, 0, gap},
		ExpectedHit{13, 4, gap}, ExpectedHit{14, 0, gap}, ExpectedHit{15, 4, gap},
		ExpectedHit{17, 10, gap}, ExpectedHit{18, 8, gap}, ExpectedHit{19, 10, gap},
		ExpectedHit{20, 9, gap}, ExpectedHit{21, 0, gap}, ExpectedHit{22, 4, gap},
		ExpectedHit{23, 0, gap}, ExpectedHit{24, 4, gap}, ExpectedHit{26, 10, gap},
		ExpectedHit{27, 8, gap}, ExpectedHit{28, 10, gap}, ExpectedHit{29, 9, gap},
		ExpectedHit{33, 6, gap}, ExpectedHit{34, 0, gap}, ExpectedHit{35, 4, gap},
		ExpectedHit{36, 0, gap}, ExpectedHit{37, 0, gap},  ExpectedHit{39, 6, gap},
		ExpectedHit{40, 2, gap}, ExpectedHit{41, 6, gap}, ExpectedHit{42, 0, gap},
		ExpectedHit{43, 4, gap}, ExpectedHit{44, 0, gap}, ExpectedHit{45, 4, gap},
		ExpectedHit{47, 10, gap}, ExpectedHit{48, 8, gap}, ExpectedHit{49, 10, gap},
		ExpectedHit{50, 9, gap}, ExpectedHit{51, 0, gap}, ExpectedHit{52, 4, gap},
		ExpectedHit{53, 0, gap}, ExpectedHit{54, 4, gap}, ExpectedHit{56, 10, gap},
		ExpectedHit{57, 8, gap}, ExpectedHit{58, 10, gap}, ExpectedHit{59, 9, gap},
		ExpectedHit{62, 8, gap}
	};
public:
	AudioLogic();
	~AudioLogic();
	void converteToBST(int ms, int bpm, ExpectedHit& hit);
	void checkHit(ExpectedHit target, ExpectedHit user, int bpm);
	ExpectedHit getHit(int index);
	void updateTime(int currentMs, int bpm);
	int convertBSTtoMs(const ExpectedHit& hit, int bpm);
	int getPERFECTW() const;
	int getGOODW() const;

};


