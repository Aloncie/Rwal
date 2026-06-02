#pragma once
#include "logs/logs.hpp"
#include "internal/filesystem/IFileSystem.hpp"

#include <gmock/gmock.h>
#include <string>

namespace fs = std::filesystem;

class MockLogs : public Logs {
public:
	std::string lastLogMessage;
	
	explicit MockLogs(IFileSystem& fs) : Logs(fs) {}
	MOCK_METHOD(void, writeLogs, (std::string_view type, std::string_view module, std::string_view message), (override));
	MOCK_METHOD(std::string, getLogs, (const int& LinesCount), (const, override));
	MOCK_METHOD(bool, refresh, (), (override));

	bool contains(std::string_view substring) {
		return lastLogMessage.find(substring) != std::string::npos;
	}
};

