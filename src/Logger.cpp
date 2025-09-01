#include "Logger.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

void Logger::log(LogLevel level, const std::string& message) {
    std::ostream& stream = getOutputStream(level);
    stream << "[" << levelToString(level) << "] " << message << std::endl;
}

void Logger::logSDLError(LogLevel level, const std::string& context) {
    log(level, context + ": " + std::string(SDL_GetError()));
}

void Logger::logSDLImageError(LogLevel level, const std::string& context) {
    log(level, context + ": " + std::string(IMG_GetError()));
}

void Logger::logSDLTTFError(LogLevel level, const std::string& context) {
    log(level, context + ": " + std::string(TTF_GetError()));
}

void Logger::logSDLMixerError(LogLevel level, const std::string& context) {
    log(level, context + ": " + std::string(Mix_GetError()));
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::DEBUG:   return "DEBUG";
        default:                return "UNKNOWN";
    }
}

std::ostream& Logger::getOutputStream(LogLevel level) {
    return (level == LogLevel::ERROR || level == LogLevel::WARNING) ? std::cerr : std::cout;
}