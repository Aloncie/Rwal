#pragma once
#include "logs/logs.hpp"

#include <gmock/gmock.h>
#include <string_view>
#include <string>

class MockLogs : public Logs {
public:
	std::string lastLogMessage;
	
	explicit MockLogs() : Logs() {}
	MOCK_METHOD(void, writeLogs, (std::string_view message), (override));
	MOCK_METHOD(std::string, getLogs, (int LinesCount), (override));

	bool contains(std::string_view substring) {
		return lastLogMessage.find(substring) != std::string::npos;
	}
};

