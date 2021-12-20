#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
aoc::matrix<int> parse(R&& input) {
    std::vector<std::string> lines(input.begin(), input.end());
    aoc::matrix<int> grid(lines.size(), lines[0].size());

    ptrdiff_t x = 0;
    for (const auto& line : lines) {
        ptrdiff_t y = 0;
        for (char c : line) {
            grid(x, y++) = c - '0';
        }
        ++x;
    }
    return grid;
}

void for_each_neighbor(aoc::point<ptrdiff_t> p, aoc::matrix<int>& grid, auto&& f) {
    for (ptrdiff_t r = -1; r <= 1; ++r) {
        for (ptrdiff_t c = -1; c <= 1; ++c) {
            const aoc::point<ptrdiff_t> offset{c, r};
            const auto p2 = p + aoc::point<ptrdiff_t>{c, r};
            if (offset != aoc::point<ptrdiff_t>{0, 0} && p2.x() >= 0 &&
                p2.x() < grid.y() && p2.y() >= 0 && p2.y() < grid.x()) {
                std::invoke(f, p2);
            }
        }
    }
}

void increment_point(aoc::point<ptrdiff_t> p, aoc::matrix<int>& grid) {
    if (++grid(p) == 10) {
        for_each_neighbor(p, grid, [&](auto n) { increment_point(n, grid); });
    }
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    auto grid = parse(AOC_FWD(input));


    size_t flashes = 0;
    for (size_t step = 0; step < 100; ++step) {
        for (ptrdiff_t x = 0; x < grid.x(); ++x) {
            for (ptrdiff_t y = 0; y < grid.y(); ++y) {
                increment_point({y, x}, grid);
            }
        }
        for (auto& val : grid.flat()) {
            if (val > 9) {
                ++flashes;
                val = 0;
            }
        }
    }
    return flashes;
}

int main() { aoc::dbg(solution(aoc::input_range<aoc::line>())); }
