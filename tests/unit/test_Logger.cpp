#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include "Logger.hpp"
#include "GameStats.hpp"

// Test fixture for Logger tests that can capture output
class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Save original stream buffers
        original_cout = std::cout.rdbuf();
        original_cerr = std::cerr.rdbuf();
        
        // Redirect cout and cerr to our string streams
        std::cout.rdbuf(cout_buffer.rdbuf());
        std::cerr.rdbuf(cerr_buffer.rdbuf());
    }
    
    void TearDown() override {
        // Restore original stream buffers
        std::cout.rdbuf(original_cout);
        std::cerr.rdbuf(original_cerr);
    }
    
    std::stringstream cout_buffer;
    std::stringstream cerr_buffer;
    std::streambuf* original_cout;
    std::streambuf* original_cerr;
};

// Test LogLevel enum values
TEST_F(LoggerTest, LogLevelEnumValues) {
    EXPECT_EQ(static_cast<int>(LogLevel::ERROR), 0);
    EXPECT_EQ(static_cast<int>(LogLevel::WARNING), 1);
    EXPECT_EQ(static_cast<int>(LogLevel::INFO), 2);
    EXPECT_EQ(static_cast<int>(LogLevel::DEBUG), 3);
}

// Test basic log functionality with different levels
TEST_F(LoggerTest, BasicLogFunctionality) {
    Logger::log(LogLevel::INFO, "Test info message");
    std::string output = cout_buffer.str();
    EXPECT_TRUE(output.find("[INFO] Test info message") != std::string::npos);
    
    cout_buffer.str(""); // Clear buffer
    cout_buffer.clear();
    
    Logger::log(LogLevel::DEBUG, "Test debug message");
    output = cout_buffer.str();
    EXPECT_TRUE(output.find("[DEBUG] Test debug message") != std::string::npos);
}

// Test error and warning messages go to stderr
TEST_F(LoggerTest, ErrorWarningToStderr) {
    Logger::log(LogLevel::ERROR, "Test error message");
    std::string error_output = cerr_buffer.str();
    EXPECT_TRUE(error_output.find("[ERROR] Test error message") != std::string::npos);
    EXPECT_TRUE(cout_buffer.str().empty()); // Should not appear in cout
    
    cerr_buffer.str(""); // Clear buffer
    cerr_buffer.clear();
    
    Logger::log(LogLevel::WARNING, "Test warning message");
    error_output = cerr_buffer.str();
    EXPECT_TRUE(error_output.find("[WARN] Test warning message") != std::string::npos);
    EXPECT_TRUE(cout_buffer.str().empty()); // Should not appear in cout
}

// Test convenience methods
TEST_F(LoggerTest, ConvenienceMethods) {
    Logger::error("Error via convenience method");
    std::string error_output = cerr_buffer.str();
    EXPECT_TRUE(error_output.find("[ERROR] Error via convenience method") != std::string::npos);
    
    cerr_buffer.str("");
    cerr_buffer.clear();
    
    Logger::warning("Warning via convenience method");
    error_output = cerr_buffer.str();
    EXPECT_TRUE(error_output.find("[WARN] Warning via convenience method") != std::string::npos);
    
    Logger::info("Info via convenience method");
    std::string info_output = cout_buffer.str();
    EXPECT_TRUE(info_output.find("[INFO] Info via convenience method") != std::string::npos);
    
    cout_buffer.str("");
    cout_buffer.clear();
    
    Logger::debug("Debug via convenience method");
    info_output = cout_buffer.str();
    EXPECT_TRUE(info_output.find("[DEBUG] Debug via convenience method") != std::string::npos);
}

// Test logObject template method
TEST_F(LoggerTest, LogObjectTemplate) {
    // Test with integer
    Logger::logObject(LogLevel::INFO, 42);
    std::string output = cout_buffer.str();
    EXPECT_TRUE(output.find("[INFO] 42") != std::string::npos);
    
    cout_buffer.str("");
    cout_buffer.clear();
    
    // Test with double
    Logger::logObject(LogLevel::DEBUG, 3.14159);
    output = cout_buffer.str();
    EXPECT_TRUE(output.find("[DEBUG] 3.14159") != std::string::npos);
    
    cout_buffer.str("");
    cout_buffer.clear();
    
    // Test with string
    std::string test_string = "Hello World";
    Logger::logObject(LogLevel::INFO, test_string);
    output = cout_buffer.str();
    EXPECT_TRUE(output.find("[INFO] Hello World") != std::string::npos);
}

// Test logObject with GameStats (assuming it has operator<< overloaded)
TEST_F(LoggerTest, LogObjectWithGameStats) {
    GameStats stats(100, 5, 8, 2);
    Logger::logObject(LogLevel::INFO, stats);
    std::string output = cout_buffer.str();
    
    // Should contain the formatted GameStats output
    EXPECT_TRUE(output.find("[INFO]") != std::string::npos);
    EXPECT_TRUE(output.find("Final Stats") != std::string::npos);
    EXPECT_TRUE(output.find("Score: 100") != std::string::npos);
}

// Test empty message handling
TEST_F(LoggerTest, EmptyMessage) {
    Logger::log(LogLevel::INFO, "");
    std::string output = cout_buffer.str();
    EXPECT_TRUE(output.find("[INFO]") != std::string::npos);
    
    Logger::info("");
    cout_buffer.str("");
    cout_buffer.clear();
    
    Logger::info("");
    output = cout_buffer.str();
    EXPECT_TRUE(output.find("[INFO]") != std::string::npos);
}

// Test message with special characters
TEST_F(LoggerTest, SpecialCharacters) {
    Logger::info("Message with newlines\nand tabs\tand symbols!@#$%");
    std::string output = cout_buffer.str();
    EXPECT_TRUE(output.find("[INFO] Message with newlines\nand tabs\tand symbols!@#$%") != std::string::npos);
}

// Test very long message
TEST_F(LoggerTest, LongMessage) {
    std::string long_message(1000, 'A');
    Logger::info(long_message);
    std::string output = cout_buffer.str();
    EXPECT_TRUE(output.find("[INFO] " + long_message) != std::string::npos);
}

// Test multiple consecutive log calls
TEST_F(LoggerTest, MultipleLogCalls) {
    Logger::info("First message");
    Logger::warning("Second message");
    Logger::debug("Third message");
    
    std::string cout_output = cout_buffer.str();
    std::string cerr_output = cerr_buffer.str();
    
    // Info and debug should be in cout
    EXPECT_TRUE(cout_output.find("[INFO] First message") != std::string::npos);
    EXPECT_TRUE(cout_output.find("[DEBUG] Third message") != std::string::npos);
    
    // Warning should be in cerr
    EXPECT_TRUE(cerr_output.find("[WARN] Second message") != std::string::npos);
}

// Test SDL error logging methods (these will test the method structure, 
// but won't test actual SDL errors since SDL may not be initialized in tests)
TEST_F(LoggerTest, SDLErrorLoggingMethods) {
    // These methods will call SDL_GetError() etc., but in a test environment
    // they might return empty strings or default error messages
    
    Logger::logSDLError(LogLevel::ERROR, "SDL initialization failed");
    std::string error_output = cerr_buffer.str();
    EXPECT_TRUE(error_output.find("[ERROR] SDL initialization failed:") != std::string::npos);
    
    cerr_buffer.str("");
    cerr_buffer.clear();
    
    Logger::logSDLImageError(LogLevel::WARNING, "Image loading failed");
    error_output = cerr_buffer.str();
    EXPECT_TRUE(error_output.find("[WARN] Image loading failed:") != std::string::npos);
    
    cerr_buffer.str("");
    cerr_buffer.clear();
    
    Logger::logSDLTTFError(LogLevel::ERROR, "Font loading failed");
    error_output = cerr_buffer.str();
    EXPECT_TRUE(error_output.find("[ERROR] Font loading failed:") != std::string::npos);
    
    cerr_buffer.str("");
    cerr_buffer.clear();
    
    Logger::logSDLMixerError(LogLevel::ERROR, "Audio initialization failed");
    error_output = cerr_buffer.str();
    EXPECT_TRUE(error_output.find("[ERROR] Audio initialization failed:") != std::string::npos);
}

// Test that log calls are thread-safe (basic test)
TEST_F(LoggerTest, BasicThreadSafety) {
    // Simple test that multiple calls don't interfere
    for (int i = 0; i < 10; ++i) {
        Logger::info("Message " + std::to_string(i));
    }
    
    std::string output = cout_buffer.str();
    
    // Check that all messages appear
    for (int i = 0; i < 10; ++i) {
        EXPECT_TRUE(output.find("Message " + std::to_string(i)) != std::string::npos);
    }
}