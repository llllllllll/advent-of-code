#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "aoc/aoc.hpp"

bool valid(int cand, const std::deque<int>& buf) {
    for (int x : buf) {
        for (int y : buf) {
            if (x != y && x + y == cand) {
                return true;
            }
        }
    }
    return false;
}

template<typename I>
int first_invalid(I begin, I end) {
    std::deque<int> buf;
    for (I it = begin; it != end; ++it) {
        int cand = aoc::parse<int>(it->get());
        if (buf.size() == 25) {
            if (!valid(cand, buf)) {
                return cand;
            }
            buf.pop_front();
        }
        buf.emplace_back(cand);
    }
    throw std::runtime_error{"no solution"};
}

int main() {
    std::cout << first_invalid(std::istream_iterator<aoc::line>{std::cin},
                               std::istream_iterator<aoc::line>{})
              << '\n';
}
