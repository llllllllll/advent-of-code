#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "aoc/aoc.hpp"

template<std::ranges::range R>
auto solution(R&& input) {
    return 0;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
