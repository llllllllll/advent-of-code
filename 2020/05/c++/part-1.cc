#include <iostream>
#include <iterator>
#include <numeric>
#include <string_view>

#include "utils/line.hpp"

int seat_location(std::string_view s) {
    int lower_row = 0;
    int upper_row = 127;

    for (char c : s.substr(0, 7)) {
        if (c == 'F') {
            upper_row = std::midpoint(lower_row, upper_row);
        }
        else {
            lower_row = std::midpoint(lower_row, upper_row) + 1;
        }
    }

    int lower_col = 0;
    int upper_col = 7;
    for (char c : s.substr(7, 3)) {
        if (c == 'L') {
            upper_col = std::midpoint(lower_col, upper_col);
        }
        else {
            lower_col = std::midpoint(lower_col, upper_col) + 1;
        }
    }

    return lower_row * 8 + lower_col;
}

template<typename I>
int max_seat_location(I begin, I end) {
    return std::accumulate(begin, end, 0, [](auto a, auto b) {
        return std::max(a, seat_location(b.get()));
    });
}

int main() {
    std::cout << seat_location("FBFBBFFRLR") << '\n';
    std::cout << max_seat_location(std::istream_iterator<aoc::line>{std::cin},
                                   std::istream_iterator<aoc::line>{})
              << '\n';
}
