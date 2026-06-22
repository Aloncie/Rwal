#pragma once
#include "mocks/MockLogs.hpp"
#include "settings/IConfigReader.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using ::testing::Return;

class MockConfigReader : public IConfigReader {
public:
    MockConfigReader(Logs& logs, IFileSystem& fs) : IConfigReader(logs, fs) {}
    ~MockConfigReader() override = default;

    MOCK_METHOD(nlohmann::json&, all, (), (override));
    MOCK_METHOD(void, reload, (), (override));
    MOCK_METHOD(nlohmann::json, getImpl, (const std::string& key), (override));
    MOCK_METHOD(bool, setImpl, (const std::string& key, const nlohmann::json& value), (override));

    // Helper to inject test data
    void setSearchKeywords(const std::vector<std::string>& keywords) {
        nlohmann::json searchJson = {
            {"search", {{"keywords", keywords}, {"sorting", "random"}, {"res", "1920x1080"}}}};
        ON_CALL(*this, getImpl("/search")).WillByDefault(Return(searchJson["search"]));
    }
};
