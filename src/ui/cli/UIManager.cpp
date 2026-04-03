#include "UIManager.hpp"
#include "logs/logs.hpp"

#include <algorithm>
#include <cctype>

bool UIManager::isInputActive() const {return inputActive;}

void UIManager::processInputChar(int ch) {
    if (!inputActive) return;

    if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) {
        std::string finalInput = inputBuffer;
        inputActive = false;
        
        move(LINES - 1, 0);
        clrtoeol();
        refresh();

        if (inputCallback) {
            inputCallback(finalInput);
        }
        return;
    }

    if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
        if (!inputBuffer.empty()) {
            inputBuffer.pop_back();
            int y, x;
            getyx(stdscr, y, x);
            move(y, x - 1);
            addch(' ');
            move(y, x - 1);
            refresh();
        }
        return;
    }

    if (ch >= 32 && ch <= 126) {
        inputBuffer.push_back(static_cast<char>(ch));
        addch(ch);
        refresh();
    }
}

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
	nodelay(stdscr, TRUE);
    curs_set(1);

	if (has_colors()) {
		start_color();
		use_default_colors();
		init_pair(1, COLOR_GREEN, -1);
        init_pair(2, COLOR_RED, -1);

	}
}

void UIManager::shutdownUI(){endwin();}

void UIManager::showMessage(std::string message) {
    if (message.empty()) return;

    move(LINES - 1, 0);
    clrtoeol();

    std::string lower = message;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    int colorPair = 0;
    if (lower.find("success") != std::string::npos) colorPair = 1;
    else if (lower.find("failed") != std::string::npos || 
             lower.find("error") != std::string::npos) colorPair = 2;

    if (colorPair != 0) attron(COLOR_PAIR(colorPair));
    
    mvprintw(LINES - 1, 0, "%s", message.c_str());
    
    if (colorPair != 0) attroff(COLOR_PAIR(colorPair));

    refresh();
}

