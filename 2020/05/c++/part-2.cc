#include <iostream>
#include <iterator>
#include <numeric>
#include <string_view>
#include <unordered_set>

#include "utils/line.hpp"

int seat_id(int row, int col) {
    return row * 8 + col;
}

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

    return seat_id(lower_row, lower_col);
}

template<typename I>
std::unordered_set<int> parse_seat_locations(I begin, I end) {
    std::unordered_set<int> out;
    for (I it = begin; it != end; ++it) {
        out.emplace(seat_location(it->get()));
    }
    return out;
}

int your_seat(const std::unordered_set<int>& seats) {
    for (int row = 0; row < 128; ++row) {
        for (int col = 0; col < 8; ++col) {
            int id = seat_id(row, col);
            if (seats.count(id) == 0 && seats.count(id - 1) + seats.count(id + 1) == 2) {
                return id;
            }
        }
    }
    return -1;
}

int main() {
    const auto seat_locations =
        parse_seat_locations(std::istream_iterator<aoc::line>{std::cin},
                             std::istream_iterator<aoc::line>{});
    std::cout << your_seat(seat_locations) << '\n';
}
