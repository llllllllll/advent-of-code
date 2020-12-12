#pragma once

#include <iostream>
#include <iterator>
#include <ranges>

#include "aoc/line.hpp"

namespace aoc {
template<typename T = aoc::line>
std::ranges::subrange<std::istream_iterator<T>> input_range() {
    return {std::istream_iterator<T>{std::cin}, std::istream_iterator<T>{}};
}
}  // namespace aoc
