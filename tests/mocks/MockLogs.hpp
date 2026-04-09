#pragma once
#include "logs/logs.hpp"

#include <gmock/gmock.h>
#include <string_view>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class MockLogs : public Logs {
public:
	std::string lastLogMessage;
	
	explicit MockLogs() : Logs() {}
	MOCK_METHOD(void, writeLogs, (std::string_view message), (override));
	MOCK_METHOD(std::string, getLogs, (const int& LinesCount), (const, override));
	MOCK_METHOD(bool, refresh, (fs::path& logs_path), (override));

	bool contains(std::string_view substring) {
		return lastLogMessage.find(substring) != std::string::npos;
	}
};

