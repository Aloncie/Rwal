#include <gtest/gtest.h>
#include "keywords/keywords.hpp"
#include "mocks/MockUIManager.hpp"
#include "mocks/MockConfig.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

class KeywordsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize the mock objects and set up any necessary expectations or return values
        mockUI = std::make_shared<MockUIManager>();
        mockConfig = std::make_shared<MockConfig>();

        // Set default return value
        mockConfig->setDefaultReturnValue();

        // Create the Keywords instance with the mock objects
        keywords std::make_unique<Keywords>(*mockUI, *mockConfig);
    }
    void TearDown() override = default;

    void setKeywords(const std::vector<std::string>& keywords) {
        nlohmann::json searchJson;
        searchJson["keywords"] = keywords;
        ON_CALL(mockConfig, getJson("search").WillByDefault(Return(searchJson["search"]));
    }

    std::shared_ptr<MockUIManager> mockUI;
    std::shared_ptr<MockConfig> mockConfig;
    std::unique_ptr<Keywords> keywords;
};

// ========== Tests ==========

// Since ShortWayGetKeywords is a template method, we need to test it with different types to ensure it behaves correctly in all cases.
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

// Since GetRandomKeywords relies on randomness, we can only test that the returned keyword is one of the expected keywords.
TEST_F(KeywordsTest, GetRandomKeywords_SingleKeyword_ReturnsIt) {
    setKeywords({"onlyone"});
    std::string capturedKeyword;

    keywords->GetRandomKeywords([&capturedKeyword](std::string kw) { capturedKeyword = kw; },"change");

    EXPECT_EQ(capturedKeyword, "onlyone");
}

TEST_F(KeywordsTest, GetRandomKeywords_Empty_FallsBackToArt) {
    setKeywords({});
    std::string capturedKeyword;

    keywords->GetRandomKeywords([&capturedKeyword](std::string kw) { capturedKeyword = kw; },"change");
	
	// Since the default keywords are used when the list is empty, we can guarantee "art" will be returned.
    EXPECT_EQ(capturedKeyword, "art");
}

// Testing the promptForKeywords method is a bit tricky since it involves user input. 
// We can simulate this by setting expectations on the mock UI and providing predefined inputs.

TEST_F(KeywordsTest, PromptForKeywords_UserEntersValidInput) {
    setKeywords({});
    std::vector<std::string> capturedKeywords;
    bool promptShown = false;

    EXPECT_CALL(*mockUI,
                requestInputCalled("Keywords not found. Enter keywords (space separated): "))
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

