#include "aoc/aoc.hpp"

struct Interval {
    int start;
    int stop;
};

constexpr auto intersects(Interval a, Interval b) -> bool {
    return not (a.stop < b.start or b.stop < a.start);
}

auto parse_interval(std::string_view interval) -> Interval {
    auto const split = aoc::split(interval, '-');
    return {aoc::parse<int>(split.at(0)), aoc::parse<int>(split.at(1))};
}

auto parse_line(std::string_view line) -> std::pair<Interval, Interval> {
    auto split = aoc::split(line, ',');
    return {parse_interval(split.at(0)), parse_interval(split.at(1))};
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    size_t count = 0;
    for (auto&& line : input) {
        auto const assignments = parse_line(line);
        count += intersects(assignments.first, assignments.second);
    }
    return count;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
