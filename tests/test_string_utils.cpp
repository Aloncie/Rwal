#include <gtest/gtest.h>
#include "internal/utils/string_utils.hpp"

TEST(StringUtilsTest, FormatRemovesWhitespace) {
    std::string input = "  testing message  ";
    rwal::utils::string::format(input);
    EXPECT_EQ(input, "testing message");
}

/*
TEST(StringUtilsTest, SplitRemovesWhitespace){
	const std::string input = "testing message";
	std::vector<std::string> result = rwal::utils::string::split_by_space(input);
	EXPECT_EQ(result, {"testing", "message"};
}

*/
