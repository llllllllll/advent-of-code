#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <set>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "aoc/aoc.hpp"

std::size_t count_arrangements(const std::set<int>& order) {
    const int end = *std::prev(order.end());
    std::vector<std::size_t> memo(end + 1);
    memo[0] = 1;
    memo[1] = order.count(1);
    if (order.count(2)) {
        memo[2] = 1 + order.count(1);
    }

    for (int ix = 3; ix <= end; ++ix) {
        if (order.count(ix)) {
            memo[ix] = memo[ix - 1] + memo[ix - 2] + memo[ix - 3];
        }
    }
    return memo[end];
}

int main() {
    std::set<int> adapters{std::istream_iterator<int>{std::cin},
                           std::istream_iterator<int>{}};
    adapters.insert(*std::prev(adapters.end()) + 3);
    std::cout << count_arrangements(adapters) << '\n';
}
