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
			if (!first_attempt) {
				showMessage("Error: Invalid or dirty input. Try again.");
			}
			first_attempt = false;
			
			input = readInput(message);
			if constexpr (std::is_same_v<T, std::string>) {
				return input;
			} else {
				std::stringstream ss(input);
				T result;

				if ((ss >> result) && (ss >> std::ws).eof()) {
					return result;
				}
			}
		}
	}
};


