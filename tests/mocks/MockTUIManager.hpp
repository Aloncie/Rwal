#pragma once
#include <gmock/gmock.h>
#include "ui/tui/TUIManager.hpp"

class MockTUIManager : public TUIManager {
public:
	MockUIManager() = default;
    MOCK_METHOD(void, showMessage, (std::string_view message), (override));
    MOCK_METHOD(void, dodgeMessage, (std::string message), (override));
    MOCK_METHOD(bool, isInputActive, (), (const, override));
    MOCK_METHOD(void, processInputChar, (int ch), (override));
	MOCK_METHOD(void, requestInputCalled, (std::string message), ());

	void requestInput(std::function<void(std::string)> callback, std::optional<std::string> message = std::nullopt) override {
		lastStringCallback_ = callback;
		if (message) {
			lastMessage_ = *message;
			requestInputCalled(lastMessage_);
		}
	}

    void simulateStringInput(std::string input) {
        if (lastStringCallback_) {
            lastStringCallback_(input);
        }
    }

    void simulateIntInput(int input) {
        if (lastIntCallback_) {
            lastIntCallback_(input);
        }
    }

    void simulateInput(std::string input) { simulateStringInput(input); }

    void simulateInput(int input) { simulateIntInput(input); }

private:
    std::function<void(std::string)> lastStringCallback_;
    std::function<void(int)> lastIntCallback_;
    std::string lastMessage_;
};

