#include "Utils.hpp"

int strToInt(std::string str)
{
    std::stringstream ss(str);
    int number;
    ss >> number;
    return number;
}

std::string intToStr(int number)
{
    std::stringstream ss;
    ss << number;
    return ss.str();
}

int strToHex(const std::string &str)
{
    int number = 0;
    std::stringstream ss;
    ss << std::hex << str;
    ss >> number;
    return number;
}

std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter)
{
    std::string result;
    for (size_t i = 0; i < strings.size(); ++i) {
        result += strings[i];
		if (i < strings.size() - 1) {
			result += delimiter;
		}
    }
    return result;
}





