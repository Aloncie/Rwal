#include "settings/config.hpp"
#include "mocks/MockLogs.hpp"

#include <memory>
#include <gtest/gtest.h>
#include <filesystem>
#include <QStandardPaths>
#include <QDir>
#include <fstream>
#include <nlohmann/json.hpp>
#include <QFileSystemWatcher>
#include <QCoreApplication>

class ConfigTest : public ::testing::Test {
protected:
	std::shared_ptr<MockLogs> mockLogs;
	std::unique_ptr<Config> config;

	void SetUp() override {
		mockLogs = std::make_shared<MockLogs>();
		config = std::make_unique<Config>(*mockLogs);
	}
};

// ========== Tests for getConfigPath ==========

TEST_F(ConfigTest, GetConfigPath_ReturnsNonEmptyPath) {
	std::string path = config->getConfigPath();
	EXPECT_FALSE(path.empty());
}

TEST_F(ConfigTest, GetConfigPath_ReturnsSamePath) {
	std::string path1 = config->getConfigPath();
	std::string path2 = config->getConfigPath();
	EXPECT_EQ(path1, path2);
}

TEST_F(ConfigTest, GetConfigPath_PathIsInAppConfigLocation) {
	std::string path = config->getConfigPath();
	std::filesystem::path expectedDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).toStdString();
	EXPECT_TRUE(path.find(expectedDir.string()) != std::string::npos);
}

// ========== Tests for loadConfig ==========

TEST_F(ConfigTest, LoadConfig_CreatesDefaultConfigIfNotExists) {
	std::filesystem::path configPath = config->getConfigPath();
	if (std::filesystem::exists(configPath)) {
		std::filesystem::remove(configPath);
	}
	config->loadConfig();
	EXPECT_TRUE(std::filesystem::exists(configPath));
}

TEST_F(ConfigTest, LoadConfig_ParsesExistingConfig) {
	std::filesystem::path configPath = config->getConfigPath();
	nlohmann::json testData = {{"test_key", "test_value"}};
	std::ofstream file(configPath);
	file << testData.dump(4);
	file.close();
	config->loadConfig();
	EXPECT_EQ(config->get<std::string>("/test_key"), "test_value");
}

TEST_F(ConfigTest, LoadConfig_InvalidJson_LogsError) {
	std::filesystem::path configPath = config->getConfigPath();
	std::ofstream file(configPath);
	file << "invalid json";
	file.close();
	ON_CALL(*mockLogs, writeLogs(testing::_))
		.WillByDefault(testing::Invoke([this](std::string_view message) {
			mockLogs->lastLogMessage = message;
	}));
	config->loadConfig();
	EXPECT_TRUE(mockLogs->contains("JSON Parse Error"));
}

TEST_F(ConfigTest, LoadConfig_WatcherAdded) {
	std::filesystem::path configPath = config->getConfigPath();
	config->loadConfig();
	EXPECT_TRUE(config->watcher->files().contains(QString::fromStdString(configPath)));
}

TEST_F(ConfigTest, LoadConfig_WatcherReAddedIfFileChanged) {
	std::filesystem::path configPath = config->getConfigPath();
	config->loadConfig();
	std::filesystem::last_write_time(configPath);
	config->loadConfig();
	EXPECT_TRUE(config->watcher->files().contains(QString::fromStdString(configPath)));
}

TEST_F(ConfigTest, LoadConfig_WatcherAddedIfFileDeleted) {
	std::filesystem::path configPath = config->getConfigPath();
	config->loadConfig();
	std::filesystem::remove(configPath);
	config->loadConfig();
	EXPECT_TRUE(config->watcher->files().contains(QString::fromStdString(configPath)));
}

TEST_F(ConfigTest, LoadConfig_WatcherAddedIfFileRecreated) {
	std::filesystem::path configPath = config->getConfigPath();
	config->loadConfig();
	std::filesystem::remove(configPath);
	std::ofstream file(configPath);
	file << "{}";
	file.close();
	config->loadConfig();
	EXPECT_TRUE(config->watcher->files().contains(QString::fromStdString(configPath)));
}

// ========== Tests for get and set methods ==========

TEST_F(ConfigTest, Get_Set_StringValue) {
	bool setResult = config->set("/test_string", "test_value");
	EXPECT_TRUE(setResult);
	std::string getResult = config->get<std::string>("/test_string");
	EXPECT_EQ(getResult, "test_value");
}

TEST_F(ConfigTest, Get_Set_IntValue) {
	bool setResult = config->set("/test_int", 42);
	EXPECT_TRUE(setResult);
	int getResult = config->get<int>("/test_int");
	EXPECT_EQ(getResult, 42);
}

TEST_F(ConfigTest, Get_Set_BoolValue) {
	bool setResult = config->set("/test_bool", true);
	EXPECT_TRUE(setResult);
	bool getResult = config->get<bool>("/test_bool");
	EXPECT_TRUE(getResult);
}

TEST_F(ConfigTest, Get_NonExistentKey_ReturnsDefault) {
	std::string getResult = config->get<std::string>("/non_existent_key");
	EXPECT_EQ(getResult, "");
}

