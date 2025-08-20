#pragma once
#include <string>
#include <iostream>
#include <map>
#include "/home/p1rat/code/rwal/src/keywords/keywords.h"

extern std::map<std::string,int> countStr;

void menu(char& choice,int& count);
void clear_last_lines(int& count);
void keywords_menu(char& choice, int& count);
void settings(char& choice, int& count, std::string& timer, std::string& wallpaper_local);
