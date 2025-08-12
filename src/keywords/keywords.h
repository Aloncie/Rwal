#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <cctype>

class Keywords{
public:
	std::string look_keywords();
	std::string get_keywords();
	std::vector<std::string> divide_keywords(std::string str);
	std::string choose_keyword(std::vector<std::string> keywords);
	std::string format_str(std::string& str);
};

