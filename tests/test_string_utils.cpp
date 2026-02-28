#include <gtest/gtest.h>
#include "internal/utils/string_utils.hpp"

TEST(StringUtilsTest, FormatRemovesWhitespace) {
    std::string input = "  testing message  ";
    rwal::utils::string::format(input);
    EXPECT_EQ(input, "testing message");
}
