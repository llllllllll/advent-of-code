#include "aoc/aoc.hpp"

int main() {
    const auto input = aoc::input_vector<int>();
    size_t count = 0;
    for (size_t ix = 3; ix < input.size(); ++ix) {
        count += input[ix] > input[ix - 3];
    }
    std::cout << count << '\n';
    return 0;
}
