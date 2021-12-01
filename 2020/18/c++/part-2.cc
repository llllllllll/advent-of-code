#include "aoc/aoc.hpp"

template<std::ranges::range R>
auto solution(R&& input) {
    return 0;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
