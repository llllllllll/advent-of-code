#include "aoc/aoc.hpp"

auto solution(std::string_view input) {
    std::vector<int> parsed;
    int sum = 0;
    size_t count = 0;
    aoc::for_split(input, ',', [&](auto cs) {
        const auto i = parsed.emplace_back(aoc::parse<int>(cs));
        sum += i;
        ++count;
    });
    const auto mean = static_cast<int>(static_cast<double>(sum) / count);
    return std::accumulate(parsed.begin(), parsed.end(), 0, [=](auto a, auto b) {
        const auto dist = std::abs(b - mean);
        const auto fuel = (dist * (dist + 1)) / 2;
        return a + fuel;
    });
}

int main() {
    aoc::dbg(solution(aoc::strip_view(aoc::all_input())));
}
