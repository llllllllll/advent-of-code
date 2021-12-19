#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
aoc::matrix<int> parse(R&& input) {
    std::vector<std::string> lines(input.begin(), input.end());

    aoc::matrix<int> grid(lines.size() + 2, lines[0].size() + 2, 10);
    ptrdiff_t row = 1;
    for (const auto& line : lines) {
        ptrdiff_t col = 1;
        for (char c : line) {
            grid(row, col++) = c - '0';
        }
        ++row;
    }

    return grid;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    auto grid = parse(AOC_FWD(input));

    int out = 0;
    for (ptrdiff_t row = 1; row < grid.rows() - 1; ++row) {
        for (ptrdiff_t col = 1; col < grid.cols() - 1; ++col) {
            const std::complex<ptrdiff_t> point{row, col};
            const auto value = grid(row, col);
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
