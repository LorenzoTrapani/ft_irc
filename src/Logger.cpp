#include "Logger.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

const std::string Logger::RESET = "\033[0m";
const std::string Logger::RED = "\033[31m";
const std::string Logger::GREEN = "\033[32m";
const std::string Logger::YELLOW = "\033[33m";
const std::string Logger::BLUE = "\033[34m";

std::string Logger::getTimestamp()
{
    std::time_t now = std::time(NULL);
    std::tm* tm = std::localtime(&now);
    
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
    
    return std::string(buffer);
}

std::string Logger::getLevelColor(LogLevel level)
{
    switch (level)
    {
        case INFO: return GREEN;
        case WARNING: return YELLOW;
        case ERROR: return RED;
        case DEBUG: return BLUE;
        default: return RESET;
    }
}

std::string Logger::getLevelString(LogLevel level)
{
    switch (level)
    {
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        case DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message)
{
    std::string color = getLevelColor(level);
    std::string levelStr = getLevelString(level);
    std::string timestamp = getTimestamp();
    
    std::cout << color << "[" << timestamp << "][" << levelStr << "] "
              << message << RESET << std::endl;
}

void Logger::info(const std::string& message)
{
    log(INFO, message);
}

void Logger::warning(const std::string& message)
{
    log(WARNING, message);
}

void Logger::error(const std::string& message)
{
    log(ERROR, message);
}

void Logger::debug(const std::string& message)
{
    log(DEBUG, message);
}