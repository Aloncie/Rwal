#include "UIManager.hpp"
#include "logs/logs.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

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

void UIManager::showMessage(std::string_view message) {
    if (message.empty()) return;

    move(LINES - 1, 0);
    clrtoeol();

    std::string lower(message);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    int colorPair = 0;
    if (lower.find("success") != std::string::npos) colorPair = 1;
    else if (lower.find("failed") != std::string::npos || 
             lower.find("error") != std::string::npos) colorPair = 2;

    if (colorPair != 0) attron(COLOR_PAIR(colorPair));
    
    mvprintw(LINES - 1, 0, "%s", lower.c_str());
    
    if (colorPair != 0) attroff(COLOR_PAIR(colorPair));

    refresh();
}

void UIManager::requestInputString(std::function<void(std::string)> callback, std::optional<std::string> message) {
	if (message) {
		showMessage(*message);
	}
	if (inputActive) return;

	prompt = message.value_or("");
	inputBuffer.clear();
	inputActive = true;
	inputCallback = [this, callback](std::string raw){
		callback(raw);
		move(LINES - 1, 0);
		clrtoeol();
		refresh();
		inputActive = false;
	};
	move(LINES - 1, 0);
	clrtoeol();
	printw("%s", prompt.c_str());
	refresh();
}

void UIManager::requestInputInt(std::function<void(int)> callback, std::optional<std::string> message) {
	if (message) {
		showMessage(*message);
	}
	if (inputActive) return;
	prompt = message.value_or("");
	inputBuffer.clear();
	inputActive = true;
	inputCallback = [this, callback](std::string raw){
		std::stringstream ss(raw);
		int result;
		if ((ss >> result) && (ss >> std::ws).eof()) {
			callback(result);
		} else {
			showMessage("Failed input. Try again.");
			inputActive = false;
			requestInputInt(callback, prompt);
			return;
		}
		move(LINES - 1, 0);
		clrtoeol();
		refresh();
		inputActive = false;
	};
	move(LINES - 1, 0);
	clrtoeol();
	printw("%s", prompt.c_str());
	refresh();
}
