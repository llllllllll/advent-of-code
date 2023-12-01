#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
auto solution(R&& input) {
    int depth = 0;
    int horizontal = 0;
    int aim = 0;
    for (const auto& command : input) {
        const auto split = aoc::split(command, ' ');
        const auto value = aoc::parse<int>(split[1]);
        if (split[0] == "forward"sv) {
            horizontal += value;
            depth += aim * value;
        }
        else if (split[0] == "down"sv) {
            aim += value;
        }
        else {
            aim -= value;
        }
    }
    return depth * horizontal;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
