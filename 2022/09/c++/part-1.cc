#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
auto solution(R&& input) {
    using Point = aoc::point<ptrdiff_t>;
    std::unordered_set<Point> visited_points;

    for (std::string_view line : input) {
        char const direction = line[0];
        auto const amount = aoc::parse<ptrdiff_t>(line.substr(2));
    }
    return 0;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
