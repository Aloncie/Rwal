#pragma once
#include <string>

struct MenuConfig;

class MenuManager{
private:
	int& count_ref;
public:
	MenuManager(int& count);
	char charactarInput(const MenuConfig& config);
	void clear_last_lines();
	std::string arrowInput(const MenuConfig& config);
	void countOperatorPlus(int count);
	void show_message(std::string message);
};
