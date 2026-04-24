#pragma once
#include <gmock/gmock.h>
#include "ui/IUserInterface.hpp"

class MockTUIManager : public IUserInterface {
public:
    MockTUIManager() = default;

    // IUserInterface overrides
    MOCK_METHOD(void, showMessage, (std::string_view message), (override));
    MOCK_METHOD(bool, prepareForExternalCommand, (), (override));
    MOCK_METHOD(bool, refresh, (), (override));

    void requestInput(std::function<void(std::string)> callback,
                      std::optional<std::string> message = std::nullopt) override {
        lastStringCallback_ = callback;
        if (message) {
            lastMessage_ = *message;
            requestInputCalled(lastMessage_);
        }
    }

    // TUI-specific methods (not in IUserInterface)
    MOCK_METHOD(void, dodgeMessage, (std::string message));
    MOCK_METHOD(bool, isInputActive, (), (const));
    MOCK_METHOD(void, processInputChar, (int ch));

    // Helper for test expectations
    MOCK_METHOD(void, requestInputCalled, (std::string message), ());

    // Simulate methods
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
