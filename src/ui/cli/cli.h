#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class MenuManager{
private:
	int count_ref;
	static std::vector<std::string> dontShowAgain;
	MenuManager();
public:
	static MenuManager& getInstatce();
	void clear_last_lines();
	void countOperatorPlus(int count);
	void show_message(std::string message);
	void dodgeMessage(std::string message);

	template<typename T>
	T request_input(std::string message) {
		std::string input;
		bool first_attempt = true;
		count_ref++;

		while (true) {
			if (!first_attempt) {
				show_message("Error: Invalid or dirty input. Try again.");
			}
			first_attempt = false;

			show_message(message);
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


