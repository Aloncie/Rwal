#pragma once
#include <string>
#include <vector>

extern const std::string keywords_path;

class Keywords{
public:
	std::string look_keywords();
	std::string get_keywords();
	std::vector<std::string> divide_keywords(std::string str);
	std::string format_str(std::string& str);
	void open_keywords_editor();
};

