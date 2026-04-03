#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <optional>
#include <ncurses.h>
#include <functional>

class UIManager{
private:
	static std::vector<std::string> dontShowAgain;
	bool inputActive = false;
	std::string inputBuffer;
	std::string prompt;
	std::function<void(std::string)> inputCallback;
public:
	void initUI();
	void shutdownUI();

	UIManager();
	virtual ~UIManager() = default;
	virtual void showMessage(std::string message);
	virtual void dodgeMessage(std::string message);
	virtual bool isInputActive() const;
	virtual void processInputChar(int ch);

	template<typename T>
	void requestInput(std::function<void(T)> callback, std::optional<std::string> message = std::nullopt) {
		if (inputActive) return;

		prompt = message.value_or("");
		inputBuffer.clear();
		inputActive = true;
		inputCallback = [this, callback](std::string raw){
			if constexpr (std::is_same_v<T, std::string>)
				callback(raw);
			else{
				std::stringstream ss(raw);
				T result;
				if ((ss >> result)&&(ss >> std::ws).eof())
					callback(result);
				else{
					showMessage("Failed input. Try again.");
					inputActive = false;
					requestInput(callback, prompt);
					return;
				}
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
};


