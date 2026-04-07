#pragma once
#include "logs/logs.hpp"

#include <gmock/gmock.h>
#include <string_view>
#include <string>

class MockLogs : public Logs {
public:
	std::string lastLogMessage;

	MOCK_METHOD(void, writeLogs, (std::string_view message), (override));

	bool contains(std::string_view substring) {
		return lastLogMessage.find(substring) != std::string::npos;
	}
};

