#include "Utils.hpp"

int strToInt(std::string str)
{
    std::stringstream ss(str);
    int number;
    ss >> number;
    return number;
}

int strToHex(const std::string &str)
{
    int number = 0;
    std::stringstream ss;
    ss << std::hex << str;
    ss >> number;
    return number;
}

std::string intToStr(int number)
{
    std::stringstream ss;
    ss << number;
    return ss.str();
}