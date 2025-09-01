#pragma once

#include <stdexcept>
#include <string>

class InitializationException : public std::runtime_error {
public:
    explicit InitializationException(const std::string& message) 
        : std::runtime_error("Initialization failed: " + message) {}
};

class ResourceLoadException : public std::runtime_error {
public:
    explicit ResourceLoadException(const std::string& message)
        : std::runtime_error("Resource load failed: " + message) {}
};

class InvalidStateException : public std::logic_error {
public:
    explicit InvalidStateException(const std::string& message)
        : std::logic_error("Invalid state: " + message) {}
};