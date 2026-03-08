#include <algorithm>
#include <cctype>
#include <map>
#include "UIManager.hpp"

bool UIManager::isInputActive() const {return inputActive;}

void UIManager::processInputChar(int ch) {
    if (!inputActive) return;

    if (ch == '\n' || ch == KEY_ENTER) {
        inputActive = false;
        move(LINES - 1, 0);
        clrtoeol();
        refresh();

        if (inputCallback) {
            inputCallback(inputBuffer);
        }
        return;
    }

    if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
        if (!inputBuffer.empty()) {
            inputBuffer.pop_back();

            move(LINES - 1, prompt.length() + inputBuffer.length());
            addch(' ');
            move(LINES - 1, prompt.length() + inputBuffer.length());
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
    curs_set(1);

	if (has_colors()) {
		start_color();
		use_default_colors();
	}
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


