#include "aoc/aoc.hpp"

int main() {
    const std::vector<int> input{std::istream_iterator<int>{std::cin},
                            std::istream_iterator<int>{}};
    std::vector<int> diff(input.size());
    std::adjacent_difference(input.begin(), input.end(), diff.begin());
    size_t count = 0;
    for (const auto& d : diff) {
        count += d > 0;
    }
    std::cout << count - 1 << '\n';
    return 0;
}
