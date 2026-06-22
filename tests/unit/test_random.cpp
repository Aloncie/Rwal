#include "funcs/funcs.hpp"

#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

TEST(RandomTest, RandomInRange) {
    std::vector<int> counts{-5, 0, 1, 5, 10, 1000};
    for (int count : counts) {
        if (count <= 0) {
            EXPECT_THROW(random(count), std::invalid_argument);
        } else {
            int result = random(count);
            EXPECT_GE(result, 1);
            EXPECT_LE(result, count);
        }
    }
}
