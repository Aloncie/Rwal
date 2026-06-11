#pragma once
#include "keywords/keywords.hpp"
#include "ui/IUserInterface.hpp"
#include "internal/filesystem/IFileSystem.hpp"

#include <gmock/gmock.h>
#include <string>
#include <functional>

class MockKeywords : public Keywords {
public:
	MockKeywords(IConfigReader& config, Logs& logs, IFileSystem& fs) : Keywords(config, logs, fs) {}
	~MockKeywords() override = default;

	MOCK_METHOD(std::vector<std::string>, loadKeywordsFromConfig, (), (const, override));
	MOCK_METHOD(std::string, SilentGetKeyword, (), (override));
	MOCK_METHOD(std::string, InteractiveGetKeyword, (IUserInterface& ui), (override));
	MOCK_METHOD(void, Default, (std::vector<std::string>& keywords), (const, override));
	MOCK_METHOD(void, editKeywords, (IUserInterface& ui), (override));
	MOCK_METHOD(std::string, pickRandomKeyword, (const std::vector<std::string>&), (const, override));
	MOCK_METHOD(void, promptForKeywords, (std::function<void(std::vector<std::string>)>, IUserInterface&, int), (override));
};

