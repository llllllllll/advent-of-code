#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
auto solution(R&& input) {
    constexpr auto digits = "0123456789"sv;
    int sum = 0;
    for (auto line : input) {
        char const first = line.at(line.find_first_of(digits));
        char const last = line.at(line.find_last_of(digits));
        sum += (first - '0') * 10 + (last - '0');
    }
    return sum;
}

auto main() -> int {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
