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
    std::size_t ix = 0;
    int prev = -1;
    std::unordered_map<int, int> last_seen;
    for (const int i : input) {
        last_seen[prev] = ix++;
        prev = i;
    }
    for (; ix < 30000000; ++ix) {
        auto search = last_seen.find(prev);
        if (search == last_seen.end()) {
            last_seen[prev] = ix;
            prev = 0;
        }
        else {
            prev = ix - search->second;
            search->second = ix;
        }
    }
    return prev;
}

int main() {
    const auto split = aoc::split(aoc::strip(aoc::all_input()), ',');
    aoc::dbg(solution(split | std::views::transform(aoc::parse<int>)));
}
