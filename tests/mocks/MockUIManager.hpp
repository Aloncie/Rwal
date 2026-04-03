#pragma once
#include <gmock/gmock.h>
#include "ui/cli/UIManager.hpp"

class MockUIManager : public UIManager {
public:
    // Mock non-template methods directly
    MOCK_METHOD(void, showMessage, (std::string message), (override));
    MOCK_METHOD(void, dodgeMessage, (std::string message), (override));
    MOCK_METHOD(bool, isInputActive, (), (const, override));
    MOCK_METHOD(void, processInputChar, (int ch), (override));

    // For requestInput (template) we need to provide a non-template wrapper
    // that we can mock, and store the callback for later simulation

    void requestInput(std::function<void(std::string)> callback,
                      std::optional<std::string> message = std::nullopt) {
        // Store the callback and message
        lastStringCallback_ = callback;
        lastMessage_ = message.value_or("");
        requestInputCalled(lastMessage_);
    }

    void requestInput(std::function<void(int)> callback,
                      std::optional<std::string> message = std::nullopt) {
        lastIntCallback_ = callback;
        lastMessage_ = message.value_or("");
        requestInputCalled(lastMessage_);
    }

    // Mock the "wrapper" method (non-template)
    MOCK_METHOD(void, requestInputCalled, (std::string message));
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

