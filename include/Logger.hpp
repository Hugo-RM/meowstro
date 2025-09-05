#pragma once

#include <iostream>
#include <string>
#include <sstream>

enum class LogLevel {
    ERROR,
    WARNING, 
    INFO,
    DEBUG
};

class Logger {
public:
    static void log(LogLevel level, const std::string& message);
    static void logSDLError(LogLevel level, const std::string& context);
    static void logSDLImageError(LogLevel level, const std::string& context);
    static void logSDLTTFError(LogLevel level, const std::string& context);
    static void logSDLMixerError(LogLevel level, const std::string& context);
    
    // Convenience methods
    static void error(const std::string& message) { log(LogLevel::ERROR, message); }
    static void warning(const std::string& message) { log(LogLevel::WARNING, message); }
    static void info(const std::string& message) { log(LogLevel::INFO, message); }
    static void debug(const std::string& message) { log(LogLevel::DEBUG, message); }
    
    // Template method for objects with operator<< overloaded
    template<typename T>
    static void logObject(LogLevel level, const T& obj) {
        std::ostringstream oss;
        oss << obj;
        log(level, oss.str());
    }
    
private:
    static std::string levelToString(LogLevel level);
    static std::ostream& getOutputStream(LogLevel level);
};