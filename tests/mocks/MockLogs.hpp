#pragma once
#include "logs/logs.hpp"

#include <gmock/gmock.h>
#include <string_view>
#include <string>

class MockLogs : public Logs {
public:
	void writeLogs(std::string_view message) override {
		lastLogMessage = message;
	}
	bool contains(std::string_view substring) {
		return lastLogMessage.find(substring) != std::string::npos;
	}
	std::string lastLogMessage;
};

