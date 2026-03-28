#pragma once
#include <gmock/gmock.h>
#include "settings/config.hpp"
#include <nlohmann/json.hpp>

class MockConfig : public Config {
public:
    // Mock template methods need non-template wrappers
    MOCK_METHOD(nlohmann::json, getJson, (const std::string& key));
    MOCK_METHOD(bool, setJson, (const std::string& key, const nlohmann::json& value));
	MOCK_METHOD(std::string, getConfigPath, ());

    // Template wrappers
    template <typename G>
    G get(const std::string& key) {
        nlohmann::json j = getJson(key);
        return j.get<G>();
    }

    template <typename S>
    bool set(const std::string& key, const S& value) {
        nlohmann::json jValue = value;
        return setJson(key, jValue);
    }
	
	// Mock non-template method
	MOCK_METHOD(nlohmann::json&, all, (), (override));

	void setDefaultConfig() {
		nlohmann::json defaultData = {
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
				{"keywords", {}},
				{"sorting", "random"},
				{"res", "1920x1080"}
			}},
			{"settings", {
				{"cursor-visibility", "true"}
			}}
		};
        ON_CALL(*this, getJson("search"))
            .WillByDefault(testing::Return(defaultData["search"]));
        ON_CALL(*this, all())
            .WillByDefault(testing::ReturnRef(defaultData));	
	}
};
