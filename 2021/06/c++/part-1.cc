#include "aoc/aoc.hpp"

using Time = size_t;

size_t simulate(const std::vector<Time>& fish_ages, Time until) {
    static constexpr Time cycle = 7;

    std::priority_queue<Time, std::vector<Time>, std::greater<Time>> events;
    for (const auto t : fish_ages) {
        events.push(t + 1);
    }
    for (auto now = events.top(); now <= until; events.pop(), now = events.top()) {
        events.push(now + cycle);
        events.push(now + cycle + 2);
    }
    return events.size();
}

auto solution(std::string_view input) {
    constexpr Time until = 80;
    std::vector<Time> fish_ages;
    aoc::for_split(aoc::strip_view(input), ',', [&](auto cs) {
        fish_ages.push_back(aoc::parse<Time>(cs));
    });
    return simulate(fish_ages, until);
}

int main() {
    aoc::dbg(solution(aoc::all_input()));
}
