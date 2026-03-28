#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory> 
#include "keywords/keywords.hpp"
#include "mocks/MockUIManager.hpp"
#include "mocks/MockConfig.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

class KeywordsTest : public ::testing::Test {
protected:
    void SetUp() override {
		// Create mock instances
        mockUI = std::make_shared<MockUIManager>();
        mockConfig = std::make_shared<MockConfig>();

        mockConfig->setDefaultConfig();

        keywords = std::make_unique<Keywords>(*mockUI, *mockConfig);
    }

    void TearDown() override {
        // Clean up if needed; default is fine
    }

    void setKeywords(const std::vector<std::string>& kw) {
        nlohmann::json searchJson;
        searchJson["keywords"] = kw;
        ON_CALL(*mockConfig, getJson("search"))
            .WillByDefault(Return(searchJson));
    }

    std::shared_ptr<MockUIManager> mockUI;
    std::shared_ptr<MockConfig> mockConfig;
    std::unique_ptr<Keywords> keywords;
};

// ========== Tests ==========

TEST_F(KeywordsTest, ShortWayGetKeywords_ReturnsKeywords) {
    std::vector<std::string> expected = {"nature", "anime", "cars"};
    setKeywords(expected);

    auto result = keywords->ShortWayGetKeywords<std::vector<std::string>>();

    EXPECT_EQ(result, expected);
}

TEST_F(KeywordsTest, ShortWayGetKeywords_Empty_ReturnsEmpty) {
    setKeywords({});

    auto result = keywords->ShortWayGetKeywords<std::vector<std::string>>();

    EXPECT_TRUE(result.empty());
}

TEST_F(KeywordsTest, ShortWayGetKeywords_AsString_ReturnsCommaSeparated) {
    setKeywords({"nature", "anime"});

    std::string result = keywords->ShortWayGetKeywords<std::string>();

    EXPECT_EQ(result, "nature, anime");
}

TEST_F(KeywordsTest, GetRandomKeywords_SingleKeyword_ReturnsIt) {
    setKeywords({"onlyone"});
    std::string capturedKeyword;

    keywords->GetRandomKeywords([&capturedKeyword](std::string kw) { capturedKeyword = kw; }, "change");

    EXPECT_EQ(capturedKeyword, "onlyone");
}

TEST_F(KeywordsTest, GetRandomKeywords_Empty_FallsBackToArt) {
    setKeywords({});
    std::string capturedKeyword;

    keywords->GetRandomKeywords([&capturedKeyword](std::string kw) { capturedKeyword = kw; }, "change");

    EXPECT_EQ(capturedKeyword, "art");
}

TEST_F(KeywordsTest, PromptForKeywords_UserEntersValidInput) {
    setKeywords({});
    std::vector<std::string> capturedKeywords;

    EXPECT_CALL(*mockUI, requestInputCalled("Keywords not found. Enter keywords (space separated): "))
        .WillOnce(Invoke([this](std::string) { mockUI->simulateInput("nature anime cars"); }));

    keywords->LongWayGetKeywords(
        [&capturedKeywords](std::vector<std::string> kw) { capturedKeywords = kw; });

    std::vector<std::string> expected = {"nature", "anime", "cars"};
    EXPECT_EQ(capturedKeywords, expected);
}

TEST_F(KeywordsTest, PromptForKeywords_UserEntersEmpty_Retries) {
    setKeywords({});
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

    std::vector<std::string> result;
    keywords->LongWayGetKeywords([&result](std::vector<std::string> kw) { result = kw; });

    EXPECT_EQ(callCount, 2);
    EXPECT_EQ(result, std::vector<std::string>({"nature"}));
}
