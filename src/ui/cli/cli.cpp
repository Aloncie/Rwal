#include <algorithm>
#include <cctype>
#include <map>
#include "cli.hpp"
#include <iostream>

void MenuManager::dodgeMessage(std::string message){
	if (std::find(dontShowAgain.begin(),dontShowAgain.end(),message) != dontShowAgain.end())
		return;
	dontShowAgain.push_back(message);
}

MenuManager::MenuManager(){}

void initUI(){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
    }
}

void MenuManager::shutdownUI(){endwin();}

void MenuManager::showMessage(std::string message){
    if (std::find(dontShowAgain.begin(), dontShowAgain.end(), message) != dontShowAgain.end())
        return;

    std::string lower = message;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    int colorPair = 0;
    if (lower.find("successful") != std::string::npos) {
        colorPair = 1; // green
    } else if (lower.find("failed") != std::string::npos ||
               lower.find("error") != std::string::npos ||
               lower.find("invalid") != std::string::npos) {
        colorPair = 2; // red
    }

    if (colorPair != 0) {
        attron(COLOR_PAIR(colorPair));
    }
    mvprintw(LINES - 1, 0, "%s", message.c_str());
    clrtoeol();
    if (colorPair != 0) {
        attroff(COLOR_PAIR(colorPair));
    }
    refresh();
}


