#pragma once
#include <string>
#include <vector>
#include <optional>
#include <ncurses.h>
#include <functional>

class TUIManager{
private:
	static std::vector<std::string> dontShowAgain;
	bool inputActive = false;
	std::string inputBuffer;
	std::string prompt;
	std::function<void(std::string)> inputCallback;
public:
	void initUI();
	void shutdownUI();

	TUIManager();
	virtual ~TUIManager() = default;
	virtual void showMessage(std::string_view message);
	virtual void dodgeMessage(std::string message);
	virtual bool isInputActive() const;
	virtual void processInputChar(int ch);

	virtual void requestInputString(std::function<void(std::string)> callback, std::optional<std::string> message = std::nullopt);
    virtual void requestInputInt(std::function<void(int)> callback, std::optional<std::string> message = std::nullopt);
};


