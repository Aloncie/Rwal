#pragma once
#include "ui/IUserInterface.hpp"

#include <string>
#include <vector>
#include <optional>
#include <ncurses.h>
#include <functional>

class TUIManager : public IUserInterface{
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
	void showMessage(std::string_view message) override;
	virtual void dodgeMessage(std::string message);
	virtual bool isInputActive() const;
	virtual void processInputChar(int ch);

	bool refresh() override;
	bool prepareForExternalCommand() override;

	void requestInput(std::function<void(std::string)> callback, std::optional<std::string> message = std::nullopt) override;
};


