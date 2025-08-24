#pragma once
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <ncurses.h>
#include "Menus.h"

extern std::map<std::string,int> countStr;

class MenuManager{
private:
	int& count_ref;
public:
	MenuManager(int& count);
	char charactarInput(const MenuConfig& config);
	void clear_last_lines();
	std::string arrowInput(const MenuConfig& config);
	void countOperatorPlus(int count);
};
