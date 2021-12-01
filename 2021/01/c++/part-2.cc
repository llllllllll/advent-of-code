#include "aoc/aoc.hpp"

int main() {
    const auto input = aoc::input_vector<int>();
    std::vector<int> sliding_sum(input.size());
    std::deque<int> buf = {input[0], input[1], input[2]};
    int sum = input[0] + input[1] + input[2];
    size_t count = 0;
    for (size_t ix = 3; ix < input.size(); ++ix) {
        const int prev_sum = sum;
        sum -= buf.front();
        buf.pop_front();
        sum += input[ix];
        buf.push_back(input[ix]);
        count += sum > prev_sum;
    }
    std::cout << count << '\n';
    return 0;
}
