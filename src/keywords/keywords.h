#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <cctype>
#include <sys/stat.h>
#include <unistd.h>

extern const std::string keywords_path;

class Keywords{
public:
	std::string look_keywords();
	std::string get_keywords(int& count);
	std::vector<std::string> divide_keywords(std::string str);
	std::string format_str(std::string& str);
	void open_keywords_editor();
};

