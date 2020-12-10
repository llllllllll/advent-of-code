#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "aoc/aoc.hpp"

using namespace std::literals;

TEST(string, split_basic_char) {
    const std::string input = "a|b|c|d";
    const auto actual = aoc::split(input, '|');
    const std::vector expected = {"a"s, "b"s, "c"s, "d"s};

    EXPECT_EQ(actual, expected);
}

TEST(string, split_basic_str) {
    const std::string input = "a|b|c|d";
    const auto actual = aoc::split(input, "|"sv);
    const std::vector expected = {"a"s, "b"s, "c"s, "d"s};

    EXPECT_EQ(actual, expected);
}

TEST(string, split_empty) {
    const std::string input = "";
    const auto actual = aoc::split(input, '|');
    const std::vector expected = {""s};

    EXPECT_EQ(actual, expected);
}

TEST(string, split_starts_delim) {
    const std::string input = "|a|b|c";
    const auto actual = aoc::split(input, '|');
    const std::vector expected = {""s, "a"s, "b"s, "c"s};

    EXPECT_EQ(actual, expected);
}


TEST(string, split_ends_delim) {
    const std::string input = "a|b|c|";
    const auto actual = aoc::split(input, '|');
    const std::vector expected = {"a"s, "b"s, "c"s, ""s};

    EXPECT_EQ(actual, expected);
}

TEST(string, split_repeated_delim) {
    const std::string input = "a|||b";
    const auto actual = aoc::split(input, '|');
    const std::vector expected = {"a"s, ""s, ""s, "b"s};

    EXPECT_EQ(actual, expected);
}

TEST(string, split_multi_char_delim) {
    const std::string input = "a-|-b-|-c";
    const auto actual = aoc::split(input, "-|-"sv);
    const std::vector expected = {"a"s, "b"s, "c"s};

    EXPECT_EQ(actual, expected);
}

T
