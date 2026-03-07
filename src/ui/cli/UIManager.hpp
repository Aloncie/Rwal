#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <optional>
#include <ncurses.h>

class UIManager{
private:
	static std::vector<std::string> dontShowAgain;
	std::string readInput(std::optional<std::string> message = std::nullopt);
public:
	void initUI();
	void shutdownUI();

	UIManager();
	void showMessage(std::string message);
	void dodgeMessage(std::string message);

	template<typename T>
	T requestInput(std::optional<std::string> message = std::nullopt) {
		std::string input;
		bool first_attempt = true;

		while (true) {
			if (!first_attempt)
				showMessage("Error: Invalid input. Try again.");
			first_attempt = false;
			
			// Clear input line
			move(LINES - 1, 0);
			clrtoeol();
			
			// Show prompt
			if (message) {
				printw("%s", message->c_str());
			}
			refresh();
			
			// Manual character input (no echo)
			input.clear();
			echo();       // Enable echo
			
			char buffer[256];
			getnstr(buffer, sizeof(buffer) - 1);
			input = buffer;
			
			noecho();
			
			// Clear the input line
			move(LINES - 1, 0);
			clrtoeol();
			refresh();
			
			if constexpr (std::is_same_v<T, std::string>)
				return input;
			else {
				std::stringstream ss(input);
				T result;
				
				if ((ss >> result) && (ss >> std::ws).eof()) 
					return result;
			}
		}
	}
};

