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
#include <ranges>

#include "aoc/aoc.hpp"

template<std::ranges::range R>
auto solution(R&& input) {
    const auto earliest = aoc::parse<int>(*std::ranges::begin(input));
    const auto cs = aoc::split(*++std::ranges::begin(input), ',');
    auto b = cs
        | std::views::filter([](std::string_view v) { return v != "x"; })
        | std::views::transform(aoc::parse<int>);
    std::vector<int> buses{b.begin(), b.end()};
    for (int i = earliest; ; ++i) {
        for (int bus : buses) {
            if (i % bus == 0) {
                return bus * (i - earliest);
            }
        }
    }
}

int main() {
    aoc::dbg(solution(aoc::input_range<std::string>()));
}
