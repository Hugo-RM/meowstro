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

    // Interpolates a position between startX and targetX over travelDurationMs,
    // timed so progress reaches 1.0 exactly at beatMs (using the same clock as
    // checkHit). Progress before the travel window starts is clamped to
    // startX; progress past beatMs keeps extrapolating at the same rate, so a
    // missed target keeps moving smoothly instead of snapping or stopping.
    static double interpolatePosition(double currentTimeMs, double beatMs, double travelDurationMs, double startX, double targetX);
private:
    const double PERFECT_WINDOW_MS = 60.0;
    const double GOOD_WINDOW_MS = 120.0;
};
