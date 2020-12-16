#pragma once

#include <iostream>
#include <iterator>
#include <ranges>
#include <sstream>

#include "aoc/line.hpp"

namespace aoc {
template<typename T = aoc::line>
std::ranges::subrange<std::istream_iterator<T>> input_range() {
    return {std::istream_iterator<T>{std::cin}, std::istream_iterator<T>{}};
}

std::string all_input() {
    std::stringstream s;
    for (const auto& l : input_range<>()) {
        s << l.get() << '\n';
    }
    return s.str();
}
}  // namespace aoc
