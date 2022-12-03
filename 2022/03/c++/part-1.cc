#include "aoc/aoc.hpp"

auto score(char c) -> int {
    if ('a' <= c and c <= 'z') {
        return c - 'a' + 1;
    }
    return c - 'A' + 27;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    int out = 0;
    for (std::string_view line : input) {
        std::string first_partition{line.substr(0, line.size() / 2)};
        std::string second_partition{line.substr(line.size() / 2)};
        std::ranges::sort(first_partition);
        std::ranges::sort(second_partition);

        std::string intersection;
        std::ranges::set_intersection(first_partition,
                                      second_partition,
                                      std::back_inserter(intersection));

        out += score(intersection[0]);
    }
    return out;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
