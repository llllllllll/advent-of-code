#include "aoc/aoc.hpp"

int main() {
    const auto input = aoc::input_vector<int>();
    std::vector<int> diff(input.size());
    std::adjacent_difference(input.begin(), input.end(), diff.begin());
    size_t count = 0;
    for (const auto& d : diff) {
        count += d > 0;
    }
    std::cout << count - 1 << '\n';
    return 0;
}
