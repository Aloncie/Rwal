#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

std::string get_keywords();
std::vector<std::string> divide_keywords(std::string str);
std::string choose_keyword(std::vector<std::string> keywords);
