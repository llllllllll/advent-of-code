#include <iostream>
#include <iterator>
#include <numeric>
#include <string_view>
#include <unordered_set>

int seat_id(int row, int col) {
    return row * 8 + col;
}

int partition(int lower, int upper, std::string_view s) {
    for (char c : s.substr(0, 7)) {
        if (c == 'F' || c == 'L') {
            upper = std::midpoint(lower, upper);
        }
        else {
            lower = std::midpoint(lower, upper);
        }
    }

    return lower;
}

int seat_location(std::string_view s) {
    return seat_id(partition(0, 128, s.substr(0, 7)), partition(0, 8, s.substr(7, 3)));
}

template<typename I>
std::unordered_set<int> parse_seat_locations(I begin, I end) {
    std::unordered_set<int> out;
    for (I it = begin; it != end; ++it) {
        out.emplace(seat_location(*it));
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
        parse_seat_locations(std::istream_iterator<std::string>{std::cin},
                             std::istream_iterator<std::string>{});
    std::cout << your_seat(seat_locations) << '\n';
}
