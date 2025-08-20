#pragma once
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <ncurses.h>

extern std::map<std::string,int> countStr;

struct MenuConfig{
	const std::string valid_choices;
	std::vector<std::string>& visual;
};

class MenuManager{
private:
	int& count_ref;
public:
	MenuManager(int& count);
	char display(const MenuConfig& config);
	char arrowDisplay(const MenuConfig& config);
};
