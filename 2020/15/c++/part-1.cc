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

constexpr std::size_t target = 2020;

template<std::ranges::range R>
auto solution(R&& input) {
    std::vector<int> last_seen(target, -1);

    std::size_t ix = 1;
    int prev = *input.begin();
    for (const int i : input | std::views::drop(1)) {
        last_seen[prev] = ix++;
        prev = i;
    }
    for (; ix < target; ++ix) {
        int& search = last_seen[prev];
        if (search == -1) {
            last_seen[prev] = ix;
            prev = 0;
        }
        else {
            prev = ix - search;
            search = ix;
        }
    }
    return prev;
}

int main() {
    const auto split = aoc::split(aoc::strip(aoc::all_input()), ',');
    aoc::dbg(solution(split | std::views::transform(aoc::parse<int>)));
}
