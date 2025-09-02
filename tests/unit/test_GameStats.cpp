#include <gtest/gtest.h>
#include "GameStats.hpp"

// Basic smoke test to verify Google Test setup is working
TEST(GameStatsTest, CanCreateGameStats) {
    GameStats stats;
    EXPECT_EQ(stats.getScore(), 0);
    EXPECT_EQ(stats.getHits(), 0);
    EXPECT_EQ(stats.getMisses(), 0);
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 0.0);
}

// Test the parameterized constructor
TEST(GameStatsTest, ParameterizedConstructor) {
    GameStats stats(100, 5, 8, 2);
    EXPECT_EQ(stats.getScore(), 100);
    EXPECT_EQ(stats.getCombo(), 5);
    EXPECT_EQ(stats.getHits(), 8);
    EXPECT_EQ(stats.getMisses(), 2);
    // 8 hits out of 10 total = 80% accuracy
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 80.0);
}

// Test accuracy calculation edge case
TEST(GameStatsTest, AccuracyWithZeroHits) {
    GameStats stats(0, 0, 0, 5);
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 0.0);
}