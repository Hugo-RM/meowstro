#include <gtest/gtest.h>
#include <sstream>
#include "GameStats.hpp"

// Basic smoke test to verify Google Test setup is working
TEST(GameStatsTest, CanCreateGameStats) {
    GameStats stats;
    EXPECT_EQ(stats.getScore(), 0);
    EXPECT_EQ(stats.getHits(), 0);
    EXPECT_EQ(stats.getMisses(), 0);
    EXPECT_EQ(stats.getCombo(), 0);
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

// Test accuracy calculation edge cases
TEST(GameStatsTest, AccuracyWithZeroHits) {
    GameStats stats(0, 0, 0, 5);
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 0.0);
}

TEST(GameStatsTest, AccuracyWithZeroMisses) {
    GameStats stats(0, 0, 5, 0);
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 100.0);
}

TEST(GameStatsTest, AccuracyWithZeroHitsAndMisses) {
    GameStats stats(0, 0, 0, 0);
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 0.0);
}

// Test setters and getters
TEST(GameStatsTest, SettersAndGetters) {
    GameStats stats;
    
    stats.setScore(250);
    EXPECT_EQ(stats.getScore(), 250);
    
    stats.setCombo(15);
    EXPECT_EQ(stats.getCombo(), 15);
    
    stats.setHits(20);
    EXPECT_EQ(stats.getHits(), 20);
    
    stats.setMisses(5);
    EXPECT_EQ(stats.getMisses(), 5);
    
    stats.setAccuracy(85.5);
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 85.5);
}

// Test increaseScore functionality
TEST(GameStatsTest, IncreaseScore) {
    GameStats stats;
    stats.setScore(100);
    
    stats.increaseScore(50);
    EXPECT_EQ(stats.getScore(), 150);
    
    stats.increaseScore(25);
    EXPECT_EQ(stats.getScore(), 175);
    
    // Test with negative values
    stats.increaseScore(-25);
    EXPECT_EQ(stats.getScore(), 150);
}

// Test resetStats functionality
TEST(GameStatsTest, ResetStats) {
    GameStats stats(100, 10, 15, 3);
    EXPECT_NE(stats.getScore(), 0);
    EXPECT_NE(stats.getCombo(), 0);
    EXPECT_NE(stats.getHits(), 0);
    EXPECT_NE(stats.getMisses(), 0);
    
    stats.resetStats();
    EXPECT_EQ(stats.getScore(), 0);
    EXPECT_EQ(stats.getCombo(), 0);
    EXPECT_EQ(stats.getHits(), 0);
    EXPECT_EQ(stats.getMisses(), 0);
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 0.0);
}

// Test post-increment operator (adds hit)
TEST(GameStatsTest, PostIncrementOperator) {
    GameStats stats;
    stats.setHits(5);
    stats.setMisses(2);
    
    GameStats result = stats++;
    
    // The implementation modifies the object and returns it
    // Both result and stats should have the new values
    EXPECT_EQ(result.getHits(), 6);
    EXPECT_EQ(stats.getHits(), 6);
    EXPECT_EQ(stats.getMisses(), 2);
    
    // Accuracy should be recalculated: 6/(6+2) = 75%
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 75.0);
}

// Test post-decrement operator (adds miss)
TEST(GameStatsTest, PostDecrementOperator) {
    GameStats stats;
    stats.setHits(8);
    stats.setMisses(1);
    
    GameStats result = stats--;
    
    // The implementation modifies the object and returns it
    // Both result and stats should have the new values
    EXPECT_EQ(result.getMisses(), 2);
    EXPECT_EQ(stats.getHits(), 8);
    EXPECT_EQ(stats.getMisses(), 2);
    
    // Accuracy should be recalculated: 8/(8+2) = 80%
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 80.0);
}

// Test post-decrement with zero hits (edge case)
TEST(GameStatsTest, PostDecrementWithZeroHits) {
    GameStats stats;
    stats.setHits(0);
    stats.setMisses(2);
    
    stats--;
    
    EXPECT_EQ(stats.getHits(), 0);
    EXPECT_EQ(stats.getMisses(), 3);
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 0.0);
}

// Test stream output operator
TEST(GameStatsTest, StreamOutputOperator) {
    GameStats stats(500, 25, 18, 2);
    std::ostringstream oss;
    
    oss << stats;
    std::string output = oss.str();
    
    // Check that key information is present in output
    EXPECT_TRUE(output.find("Final Stats") != std::string::npos);
    EXPECT_TRUE(output.find("Score: 500") != std::string::npos);
    EXPECT_TRUE(output.find("Combo: 25") != std::string::npos);
    EXPECT_TRUE(output.find("Accuracy: 90") != std::string::npos);
    EXPECT_TRUE(output.find("Well done!") != std::string::npos);
}

// Test stream output with low accuracy
TEST(GameStatsTest, StreamOutputLowAccuracy) {
    GameStats stats(100, 5, 2, 8);  // 20% accuracy
    std::ostringstream oss;
    
    oss << stats;
    std::string output = oss.str();
    
    EXPECT_TRUE(output.find("Oh.. thats kinda bad..") != std::string::npos);
}

// Test accuracy calculation precision
TEST(GameStatsTest, AccuracyPrecision) {
    GameStats stats(0, 0, 1, 3);  // 1/(1+3) = 0.25 = 25%
    EXPECT_DOUBLE_EQ(stats.getAccuracy(), 25.0);
    
    GameStats stats2(0, 0, 2, 1);  // 2/(2+1) = 0.6667 = 66.67%
    EXPECT_NEAR(stats2.getAccuracy(), 66.666666666666671, 0.000001);
}