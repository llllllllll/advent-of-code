#include <iostream>
#include <iterator>
#include <numeric>
#include <string_view>

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
int max_seat_location(I begin, I end) {
    return std::accumulate(begin, end, 0, [](auto a, auto b) {
        return std::max(a, seat_location(b));
    });
}

int main() {
    std::cout << max_seat_location(std::istream_iterator<std::string>{std::cin},
                                   std::istream_iterator<std::string>{})
              << '\n';
}
