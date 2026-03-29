#pragma once
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>
#include "settings/IConfigReader.hpp"

class MockConfigReader : public IConfigReader {
public:
    MOCK_METHOD(nlohmann::json&, all, (), (override));
    MOCK_METHOD(void, reload, (), (override));
    MOCK_METHOD(nlohmann::json, getImpl, (const std::string& key), (override));
    MOCK_METHOD(bool, setImpl, (const std::string& key, const nlohmann::json& value), (override));
    // Helper to inject test data
    void setSearchKeywords(const std::vector<std::string>& keywords) {
		nlohmann::json searchJson = {
			{"services", {
        		{"wallhaven", {
            		{"apikey", "apikey="},
            		{"base_url", "https://wallhaven.cc/api/v1/search"},
            		{"param_names", {
                		{"query", "?q="},
                		{"sorting", "sorting"},
                		{"res", "resolutions"}
            	}}
        	}}
    		}},
    		{"search", {
        		{"keywords", keywords},
        		{"sorting", "random"},
        		{"res", "1920x1080"}
    		}},
			{"settings", {
				{"cursor-visibility", "true"}
			}}
		};



        ON_CALL(*this, getImpl("search")).WillByDefault(::testing::Return(searchJson));
    }
};
