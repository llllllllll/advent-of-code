#include <string>
#include <random>

#include "gtest/gtest.h"

#include "aoc/aoc.hpp"

TEST(parse, int_fuzz) {
    std::minstd_rand gen(25);
    std::uniform_int_distribution dist(std::numeric_limits<int>::min());

    bool saw_negative = false;
    for (int ix = 0; ix < 100000; ++ix) {
        const int sample = dist(gen);
        saw_negative |= sample < 0;

        const std::string str = std::to_string(sample);
        const int result = aoc::parse<int>(str);

        EXPECT_EQ(result, sample);
    }
    EXPECT_TRUE(saw_negative);
}

TEST(parse, int_plus) {
    std::minstd_rand gen(25);
    std::uniform_int_distribution dist(0);

    for (int ix = 0; ix < 100; ++ix) {
        const int sample = dist(gen);

        const std::string str = "+" + std::to_string(sample);
        const int result = aoc::parse<int>(str);

        EXPECT_EQ(result, sample);
    }
}

TEST(parse, int_leading_whitespace) {
    EXPECT_THROW(aoc::parse<int>(" 123"), aoc::bad_parse<int>);
}

TEST(parse, int_trailing_whitespace) {
    EXPECT_THROW(aoc::parse<int>("123 "), aoc::bad_parse<int>);
}

TEST(parse, int_garbage) {
    EXPECT_THROW(aoc::parse<int>("garbage"), aoc::bad_parse<int>);
}
