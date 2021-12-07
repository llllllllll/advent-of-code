#include "aoc/aoc.hpp"

auto solution(std::string_view input) {
    std::vector<int> parsed;
    aoc::for_split(input, ',', [&](auto cs) {
        parsed.push_back(aoc::parse<int>(cs));
    });
    std::sort(parsed.begin(), parsed.end());
    const auto median = parsed[parsed.size() / 2];
    return std::accumulate(parsed.begin(), parsed.end(), 0, [=](auto a, auto b) {
        return a + std::abs(b - median);
    });
}

int main() {
    aoc::dbg(solution(aoc::strip_view(aoc::all_input())));
}
