#include "settings/config.hpp"
#include "mocks/MockLogs.hpp"
#include "mocks/MockFileSystem.hpp"

#include <memory>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;

class ConfigTest : public ::testing::Test {
protected:
    std::shared_ptr<MockLogs> mockLogs;
    std::shared_ptr<MockFileSystem> mockFileSystem;
    std::unique_ptr<Config> config;

    void SetUp() override {
		mockFileSystem = std::make_shared<MockFileSystem>();
        mockLogs = std::make_shared<MockLogs>(*mockFileSystem);
        config = std::make_unique<Config>(*mockLogs, *mockFileSystem);
    }
};

TEST_F(ConfigTest, Get_Set_StringValue) {
    bool setResult = config->set("test_string", "test_value");
    EXPECT_TRUE(setResult);
    std::string getResult = config->get<std::string>("/test_string");
    EXPECT_EQ(getResult, "test_value");
}

TEST_F(ConfigTest, Get_Set_IntValue) {
    bool setResult = config->set("test_int", 42);
    EXPECT_TRUE(setResult);
    int getResult = config->get<int>("/test_int");
    EXPECT_EQ(getResult, 42);
}

TEST_F(ConfigTest, Get_Set_BoolValue) {
    bool setResult = config->set("test_bool", true);
    EXPECT_TRUE(setResult);
    bool getResult = config->get<bool>("/test_bool");
    EXPECT_TRUE(getResult);
}

TEST_F(ConfigTest, Get_NonExistentKey_ReturnsDefault) {
    std::string getResult = config->get<std::string>("/non_existent_key");
    EXPECT_EQ(getResult, "");
}
