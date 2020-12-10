#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <string_view>
#include <vector>
#include <optional>
#include <set>

#include "aoc/aoc.hpp"

template<typename I>
std::size_t num_diffs(int size, I begin, I end) {
    std::size_t out = 0;
    I it = begin;
    while (it != end) {
        it = std::adjacent_find(it, end, [size](auto a, auto b) {
            return std::abs(a - b) == size;
        });
        ++out;
        if (it != end) {
            ++it;
        }
    }
    return out;
}

std::size_t solution(const std::set<int>& as) {
    return num_diffs(1, as.begin(), as.end()) * num_diffs(3, as.begin(), as.end());
}

int main() {
    std::set<int> adapters{std::istream_iterator<int>{std::cin},
                           std::istream_iterator<int>{}};
    std::cout << solution(adapters) << '\n';
}
