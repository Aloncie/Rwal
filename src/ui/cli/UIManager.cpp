#include <algorithm>
#include <cctype>
#include <map>
#include "UIManager.hpp"
#include <iostream>

std::vector<std::string> UIManager::dontShowAgain;

void UIManager::dodgeMessage(std::string message){
	if (std::find(dontShowAgain.begin(),dontShowAgain.end(),message) != dontShowAgain.end())
		return;
	dontShowAgain.push_back(message);
}

UIManager::UIManager(){}

void UIManager::initUI(){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1);

	if (has_colors()) {
		start_color();
		use_default_colors();
	}
}

std::string UIManager::readInput(std::optional<std::string> message){
	if (message != std::nullopt){
		mvprintw(LINES - 1, 0, "%s", message->c_str());
		clrtoeol();
		refresh();
	}

	echo();
	char buffer[256];
	getnstr(buffer, sizeof(buffer)-1);
	noecho();

	return std::string(buffer);
}

void UIManager::shutdownUI(){endwin();}

void UIManager::showMessage(std::string message){
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


