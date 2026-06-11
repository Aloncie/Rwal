#include "keywords/keywords.hpp"
#include "mocks/MockTUIManager.hpp"
#include "mocks/MockConfigReader.hpp"
#include "mocks/MockLogs.hpp"
#include "mocks/MockFileSystem.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

class KeywordsTest : public ::testing::Test {
protected:
	std::shared_ptr<MockTUIManager> mockUI;
    std::shared_ptr<MockConfigReader> mockConfig;
	std::shared_ptr<MockFileSystem> mockFileSystem;
	std::shared_ptr<MockLogs> mockLogs;
    std::unique_ptr<Keywords> keywords;

    void SetUp() override {
        mockUI = std::make_shared<MockTUIManager>();
		mockLogs = std::make_shared<MockLogs>(*mockFileSystem);
        mockConfig = std::make_shared<MockConfigReader>(*mockLogs, *mockFileSystem);
        keywords = std::make_unique<Keywords>(*mockConfig, *mockLogs, *mockFileSystem);
    }
};

// ========== Tests for SilentGetKeyword ==========
TEST_F(KeywordsTest, SilentGetKeyword_ReturnsRandomFromConfig) {
    std::vector<std::string> expected = {"nature", "city", "cars"};
    mockConfig->setSearchKeywords(expected);
    std::string result = keywords->SilentGetKeyword();
    EXPECT_TRUE(result == "nature" || result == "city" || result == "cars");
}

TEST_F(KeywordsTest, SilentGetKeyword_EmptyConfig_UsesDefault) {
    mockConfig->setSearchKeywords({});
    std::string result = keywords->SilentGetKeyword();
    EXPECT_FALSE(result.empty());
}

// ========== Tests for InteractiveGetKeyword ==========
TEST_F(KeywordsTest, InteractiveGetKeyword_WithExistingKeywords_ReturnsRandom) {
    mockConfig->setSearchKeywords({"nature", "city"});
    std::string result = keywords->InteractiveGetKeyword(*mockUI);
    EXPECT_TRUE(result == "nature" || result == "city");
}

TEST_F(KeywordsTest, InteractiveGetKeyword_EmptyConfig_PromptsUser) {
    mockConfig->setSearchKeywords({});
    EXPECT_CALL(*mockUI, requestInputCalled("Keywords not found. Enter keywords (space separated): ")) .WillOnce(Invoke([this](std::string) { mockUI->simulateInput("nature city cars"); }));

    std::string result = keywords->InteractiveGetKeyword(*mockUI);
    EXPECT_TRUE(result == "nature" || result == "city" || result == "cars");
}

TEST_F(KeywordsTest, InteractiveGetKeyword_UserEntersEmpty_Retries) {
    mockConfig->setSearchKeywords({});
    int callCount = 0;

    EXPECT_CALL(*mockUI, requestInputCalled(_))
        .WillOnce(Invoke([this, &callCount](std::string) {
            callCount++;
            mockUI->simulateInput("");
        }))
        .WillOnce(Invoke([this, &callCount](std::string) {
            callCount++;
            mockUI->simulateInput("nature");
        }));

    EXPECT_CALL(*mockUI, showMessage("Input cannot be empty! Try again.")).Times(1);

    std::string result = keywords->InteractiveGetKeyword(*mockUI);
    EXPECT_EQ(callCount, 2);
    EXPECT_EQ(result, "nature");
}

// ========== Tests for Default method ==========
TEST_F(KeywordsTest, Default_PopulatesDefaultKeywords) {
    std::vector<std::string> expected = {"nature",       "landscape", "abstract", "space",
                                         "architecture", "animals",   "city",    "cars"};
    std::vector<std::string> result;
    keywords->Default(result);
    EXPECT_EQ(result, expected);
}
