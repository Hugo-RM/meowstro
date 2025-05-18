// AudioLogic.hpp
#pragma once
#include <vector>
#include <iostream>
#include <cmath>

class AudioLogic {
public:
    AudioLogic();
    ~AudioLogic();

    double getPERFECT() const;
    double getGOOD() const;

    static double msFromMscs(int minutes, int seconds, int centiseconds);
    // Timing conversion utilities
    double beatToTimestampMs(double beat, int bpm);
    double timestampMsToBeat(double timestampMs, int bpm);

    // Hit registration and evaluation
    short int checkHit(double expectedMs, double actualMs);
private:
    const double PERFECT_WINDOW_MS = 60.0;
    const double GOOD_WINDOW_MS = 120.0;
};
