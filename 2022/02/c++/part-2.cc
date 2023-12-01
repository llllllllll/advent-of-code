#include "aoc/aoc.hpp"

enum class Throw : int8_t {
    rock,
    paper,
    scissors,
};

auto parse_throw(char c) -> Throw {
    return Throw(c - 'A');
}

auto throw_score(Throw t) -> int {
    return static_cast<int8_t>(t) + 1;
}

enum class TargetResult : int8_t {
    loss = -1,
    tie = 0,
    win = +1,
};

auto parse_target(char c) -> TargetResult {
    return TargetResult(c - 'Y');
}

auto target_score(TargetResult r) -> int {
    return (static_cast<int8_t>(r) + 1) * 3;
}

auto needed_throw(Throw a, TargetResult r) {
    return Throw((static_cast<int8_t>(a) + static_cast<int8_t>(r) + 3) % 3);
}


template<std::ranges::input_range R>
auto solution(R&& input) {
    int total_score = 0;
    for (auto&& line : input) {
        auto const opponent = parse_throw(line[0]);
        auto const target = parse_target(line[2]);
        total_score +=
            throw_score(needed_throw(opponent, target)) + target_score(target);
    }
    return total_score;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
