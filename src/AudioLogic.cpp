// AudioLogic.cpp
#include "AudioLogic.hpp"

AudioLogic::AudioLogic() {
}
AudioLogic::~AudioLogic() {}

double AudioLogic::getPERFECT() const {
    return PERFECT_WINDOW_MS;
}
double AudioLogic::getGOOD() const {
    return GOOD_WINDOW_MS;
}

double AudioLogic::beatToTimestampMs(double beat, int bpm) {
    return beat * (60000.0 / bpm);
}
double AudioLogic::timestampMsToBeat(double timestampMs, int bpm) {
    return timestampMs * bpm / 60000.0;
}

void AudioLogic::checkHit(double expectedMs, double actualMs) {
    double delta = fabs(expectedMs - actualMs);

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

double AudioLogic::msFromMscs(int minutes, int seconds, int centiseconds) {
    return (minutes * 60000.0) + (seconds * 1000.0) + (centiseconds * 10.0);
}