#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
aoc::matrix<int> parse(R&& input) {
    std::vector<std::string> lines(input.begin(), input.end());

    aoc::matrix<int> grid(lines.size() + 2, lines[0].size() + 2, 10);
    ptrdiff_t x = 1;
    for (const auto& line : lines) {
        ptrdiff_t y = 1;
        for (char c : line) {
            grid(x, y++) = c - '0';
        }
        ++x;
    }

    return grid;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    auto grid = parse(AOC_FWD(input));

    int out = 0;
    for (ptrdiff_t x = 1; x < grid.x() - 1; ++x) {
        for (ptrdiff_t y = 1; y < grid.y() - 1; ++y) {
            const std::complex<ptrdiff_t> point{x, y};
            const auto value = grid(x, y);
            bool valid = true;
            for (auto offset : std::initializer_list<std::complex<ptrdiff_t>>{{+0, +1},
                                                                              {+0, -1},
                                                                              {+1, +0},
                                                                              {-1, +0}}) {
                const auto test_point = point + offset;
                if (grid(test_point.real(), test_point.imag()) <= value) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                out += 1 + value;
            }
        }
    }
    return out;
}

int main() { aoc::dbg(solution(aoc::input_range<aoc::line>())); }
