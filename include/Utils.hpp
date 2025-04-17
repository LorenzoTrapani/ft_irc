#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>
#include <vector>

int			strToInt(std::string str);
int			strToHex(const std::string &str);
std::string intToStr(int number);
std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter);

#endif