#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <string_view>
#include <vector>

#include "aoc/aoc.hpp"

template<typename I>
auto parse(I begin, I end) {
    for (I it = begin; it != end; ++it) {
        const std::string& line = it->get();
    }
}

int main() {
    parse(std::istream_iterator<aoc::line>{std::cin},
          std::istream_iterator<aoc::line>{});
}
