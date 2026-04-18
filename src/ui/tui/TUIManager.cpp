#include "TUIManager.hpp"
#include "logs/logs.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>

bool TUIManager::isInputActive() const {return inputActive;}

bool TUIManager::refresh() {
	try {
		::refresh();
	} catch (std::exception const& e) {
		std::cerr << "Failed to refresh UI: " << e.what() << std::endl;
		return false;
	}
	return true;
}

bool TUIManager::prepareForExternalCommand() {
	try {
		def_prog_mode();
		endwin();
	} catch (std::exception const& e) {	
		std::cerr << "Failed to prepare for external command: " << e.what() << std::endl;
		return false;
	}
	return true;
}
void TUIManager::processInputChar(int ch) {
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

std::vector<std::string> TUIManager::dontShowAgain;

void TUIManager::dodgeMessage(std::string message){
	if (std::find(dontShowAgain.begin(),dontShowAgain.end(),message) != dontShowAgain.end())
		return;
	dontShowAgain.push_back(message);
}

TUIManager::TUIManager(){}

void TUIManager::initUI(){
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

void TUIManager::shutdownUI(){endwin();}

void TUIManager::showMessage(std::string_view message)  {
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

void TUIManager::requestInput(std::function<void(std::string)> callback, std::optional<std::string> message)  {
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

