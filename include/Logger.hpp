#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>

class Logger
{
public:
    enum LogLevel {
        INFO,
        WARNING,
        ERROR,
        DEBUG
    };

    static void log(LogLevel level, const std::string &message);
    static void info(const std::string &message);
    static void warning(const std::string &message);
    static void error(const std::string &message);
    static void debug(const std::string &message);

private:
    static std::string getTimestamp();
    static std::string getLevelColor(LogLevel level);
    static std::string getLevelString(LogLevel level);
    static const std::string RESET;
    static const std::string RED;
    static const std::string GREEN;
    static const std::string YELLOW;
    static const std::string BLUE;
};

#endif 