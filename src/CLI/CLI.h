#pragma once
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <ncurses.h>

extern std::map<std::string,int> countStr;

struct MenuConfig{
	const std::string valid_choices;
	const std::vector<std::string>& menu;
};

class MenuManager{
private:
	int& count_ref;
public:
	MenuManager(int& count);
	char display(const MenuConfig& config);
	void clear_last_lines();
	char arrowDisplay(const MenuConfig& config);
};
