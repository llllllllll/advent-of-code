#pragma once

#include <iostream>
#include <iterator>
#include <ranges>
#include <sstream>
#include <vector>

#include "aoc/line.hpp"

namespace aoc {
template<typename T = aoc::line>
std::ranges::subrange<std::istream_iterator<T>> input_range() {
    return {std::istream_iterator<T>{std::cin}, std::istream_iterator<T>{}};
}

template<typename T = aoc::line>
std::vector<T> input_vector() {
    auto r = input_range<T>();
    return std::vector<int>{r.begin(), r.end()};
}

inline std::string all_input() {
    std::stringstream s;
    for (const auto& l : input_range<>()) {
        s << l << '\n';
    }
    return s.str();
}
}  // namespace aoc
