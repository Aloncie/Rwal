#pragma once
#include "keywords/keywords.hpp"

#include <gmock/gmock.h>
#include <string>

class MockKeywords : public Keywords {
public:
	using Keywords::Keywords;
	
	MockKeywords(IConfigReader& config, Logs& logs) : Keywords(config, logs) {}

	MOCK_METHOD(std::vector<std::string>, loadKeywordsFromConfig, (), (const, override));
	MOCK_METHOD(std::string, SilentGetKeyword, (), (override));
	MOCK_METHOD(std::string, InteractiveGetKeyword, (UIManager& ui), (override));
	MOCK_METHOD(void, Default, (std::vector<std::string>& keywords), (const, override));

	~MockKeywords() override = default;
};
