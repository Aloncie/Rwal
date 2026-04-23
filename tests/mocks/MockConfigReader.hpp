#pragma once
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>
#include "settings/IConfigReader.hpp"
#include "mocks/MockLogs.hpp"

class MockConfigReader : public IConfigReader {
public:
	using IConfigReader::IConfigReader;
	MockConfigReader() = default;
	~MockConfigReader() override = default;

    MOCK_METHOD(nlohmann::json&, all, (), (override));
    MOCK_METHOD(void, reload, (), (override));
    MOCK_METHOD(nlohmann::json, getImpl, (const std::string& key), (override));
    MOCK_METHOD(bool, setImpl, (const std::string& key, const nlohmann::json& value), (override));
	
	explicit MockConfigReader(Logs& logs) : IConfigReader(logs) {}
    // Helper to inject test data
    void setSearchKeywords(const std::vector<std::string>& keywords) {
		nlohmann::json searchJson = {
			{"search", {
				{"keywords", keywords},
				{"sorting", "random"},
				{"res", "1920x1080"}
			}}
		};
		ON_CALL(*this, getImpl("/search")).WillByDefault(Return(searchJson["search"]));
    }
};

