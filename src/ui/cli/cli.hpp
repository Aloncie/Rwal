#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <optional>
#include <ncurses.h>

class MenuManager{
private:
	static std::vector<std::string> dontShowAgain;
public:
	void initUI();
	void shutdownUI();

	MenuManager();
	void showMessage(std::string message);
	void dodgeMessage(std::string message);

	template<typename T>
	T request_input(std::optional<std::string> message = std::nullopt) {
		std::string input;
		bool first_attempt = true;

		while (true) {
			if (!first_attempt) {
				showMessage("Error: Invalid or dirty input. Try again.");
			}
			first_attempt = false;
			if (!std::getline(std::cin, input)) {
				std::cin.clear();
				continue;
			}

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


