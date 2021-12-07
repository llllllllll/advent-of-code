#include "aoc/aoc.hpp"

using Time = int64_t;

size_t fish_spawned(Time t, std::unordered_map<Time, size_t>& memo) {
    constexpr Time cycle = 7;
    if (t < 0) {
        return 0;
    }
    if (t < cycle) {
        return 1;
    }
    auto [it, inserted] = memo.try_emplace(t, 0);
    if (inserted) {
        it->second = fish_spawned(t - cycle, memo) + fish_spawned(t - (cycle + 2), memo) +
                     1;
    }
    return it->second;
}

auto solution(std::string_view input) {
    constexpr Time until = 256;
    std::unordered_map<Time, size_t> memo;
    size_t sum = 0;
    aoc::for_split(aoc::strip_view(input), ',', [&](auto cs) {
        sum += fish_spawned(until - aoc::parse<Time>(cs) - 1, memo) + 1;
    });
    return sum;
}

int main() {
    aoc::dbg(solution(aoc::all_input()));
}
