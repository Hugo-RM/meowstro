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

    // Blends a noisy, possibly-glitchy audio-position reading into a steady
    // wall-clock baseline, expressed as the offset (audio time - wall-clock
    // time) to add to future wall-clock readings. currentOffsetMs is the
    // offset from the previous frame; wallClockMs/audioReportedMs are this
    // frame's readings.
    //
    // Real audio hardware and the system clock drift apart by at most a few
    // ms per second, so a single-frame jump larger than maxJumpMs is treated
    // as a glitch (e.g. an audio buffer underrun under CPU load, or a
    // decoder position hiccup) and rejected outright rather than snapping
    // the game's timeline - which is what makes fish visibly stutter, since
    // fish position is a direct function of this clock. Genuine drift is
    // still tracked, just gradually, via smoothingFactor (0-1: how much of
    // the implied offset change to accept per frame).
    static double smoothAudioClockOffset(double currentOffsetMs, double wallClockMs, double audioReportedMs,
                                          double maxJumpMs, double smoothingFactor);
private:
    const double PERFECT_WINDOW_MS = 60.0;
    const double GOOD_WINDOW_MS = 120.0;
};
