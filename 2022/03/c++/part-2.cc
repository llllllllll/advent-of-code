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
    for (auto chunk : ranges::views::chunk(input, 3)) {
        std::vector<std::string> lines;
        for (auto& line : chunk) {
            std::ranges::sort(lines.emplace_back(std::move(line)));
        }

        std::string a_b;
        std::ranges::set_intersection(lines[0],
                                      lines[1],
                                      std::back_inserter(a_b));

        std::string intersection;
        std::ranges::set_intersection(a_b,
                                      lines[2],
                                      std::back_inserter(intersection));

        out += score(intersection[0]);
    }
    return out;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
