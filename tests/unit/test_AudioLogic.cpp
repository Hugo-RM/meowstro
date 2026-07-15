#include <gtest/gtest.h>
#include "AudioLogic.hpp"

class AudioLogicTest : public ::testing::Test {
protected:
    AudioLogic audioLogic;
};

// --- Hit windows ---

TEST_F(AudioLogicTest, HitWindowConstants) {
    EXPECT_DOUBLE_EQ(audioLogic.getPERFECT(), 60.0);
    EXPECT_DOUBLE_EQ(audioLogic.getGOOD(), 120.0);
}

TEST_F(AudioLogicTest, CheckHitPerfectAtExactTiming) {
    EXPECT_EQ(audioLogic.checkHit(1000.0, 1000.0), 2);
}

TEST_F(AudioLogicTest, CheckHitPerfectWithinWindow) {
    EXPECT_EQ(audioLogic.checkHit(1000.0, 1000.0 + 59.0), 2);
    EXPECT_EQ(audioLogic.checkHit(1000.0, 1000.0 - 60.0), 2); // Inclusive boundary
}

TEST_F(AudioLogicTest, CheckHitGoodJustOutsidePerfect) {
    EXPECT_EQ(audioLogic.checkHit(1000.0, 1000.0 + 61.0), 1);
    EXPECT_EQ(audioLogic.checkHit(1000.0, 1000.0 - 120.0), 1); // Inclusive boundary
}

TEST_F(AudioLogicTest, CheckHitMissOutsideGoodWindow) {
    EXPECT_EQ(audioLogic.checkHit(1000.0, 1000.0 + 121.0), 0);
    EXPECT_EQ(audioLogic.checkHit(1000.0, 1000.0 - 500.0), 0);
}

TEST_F(AudioLogicTest, CheckHitIsSymmetric) {
    EXPECT_EQ(audioLogic.checkHit(1000.0, 950.0), audioLogic.checkHit(950.0, 1000.0));
}

// --- Timestamp / beat conversion ---

TEST_F(AudioLogicTest, MsFromMscsConversion) {
    EXPECT_DOUBLE_EQ(AudioLogic::msFromMscs(0, 0, 0), 0.0);
    EXPECT_DOUBLE_EQ(AudioLogic::msFromMscs(0, 3, 46), 3460.0);
    EXPECT_DOUBLE_EQ(AudioLogic::msFromMscs(1, 0, 0), 60000.0);
    EXPECT_DOUBLE_EQ(AudioLogic::msFromMscs(0, 0, 99), 990.0);
}

TEST_F(AudioLogicTest, BeatToTimestampAtGivenBpm) {
    // At 120 BPM, one beat is 500ms
    EXPECT_DOUBLE_EQ(audioLogic.beatToTimestampMs(1.0, 120), 500.0);
    EXPECT_DOUBLE_EQ(audioLogic.beatToTimestampMs(4.0, 120), 2000.0);
}

TEST_F(AudioLogicTest, TimestampToBeatRoundTrip) {
    double originalBeat = 3.5;
    int bpm = 147;
    double ms = audioLogic.beatToTimestampMs(originalBeat, bpm);
    EXPECT_NEAR(audioLogic.timestampMsToBeat(ms, bpm), originalBeat, 1e-9);
}

// --- Time-based position interpolation (fish travel) ---

TEST_F(AudioLogicTest, InterpolatePositionBeforeSpawnStaysAtStart) {
    // beat at 5000ms, travel takes 2000ms -> spawns at 3000ms
    double x = AudioLogic::interpolatePosition(/*currentTime=*/1000.0, /*beat=*/5000.0,
                                                /*travelDuration=*/2000.0, /*startX=*/1000.0, /*targetX=*/100.0);
    EXPECT_DOUBLE_EQ(x, 1000.0);
}

TEST_F(AudioLogicTest, InterpolatePositionAtSpawnTimeIsStart) {
    double x = AudioLogic::interpolatePosition(3000.0, 5000.0, 2000.0, 1000.0, 100.0);
    EXPECT_DOUBLE_EQ(x, 1000.0);
}

TEST_F(AudioLogicTest, InterpolatePositionAtBeatTimeIsTarget) {
    double x = AudioLogic::interpolatePosition(5000.0, 5000.0, 2000.0, 1000.0, 100.0);
    EXPECT_NEAR(x, 100.0, 1e-9);
}

TEST_F(AudioLogicTest, InterpolatePositionHalfwayIsMidpoint) {
    // Halfway through the 2000ms travel window (currentTime = 4000ms)
    double x = AudioLogic::interpolatePosition(4000.0, 5000.0, 2000.0, 1000.0, 100.0);
    EXPECT_NEAR(x, 550.0, 1e-9);
}

TEST_F(AudioLogicTest, InterpolatePositionExtrapolatesPastBeat) {
    // 1000ms past the beat (half the travel window again) should overshoot the
    // target by the same amount it covered in the first half of the window.
    double x = AudioLogic::interpolatePosition(6000.0, 5000.0, 2000.0, 1000.0, 100.0);
    EXPECT_NEAR(x, -350.0, 1e-9);
}

TEST_F(AudioLogicTest, InterpolatePositionZeroTravelDurationReturnsTarget) {
    double x = AudioLogic::interpolatePosition(1000.0, 1000.0, 0.0, 1000.0, 100.0);
    EXPECT_DOUBLE_EQ(x, 100.0);
}
