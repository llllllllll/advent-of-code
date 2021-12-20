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

void remove_basin(aoc::point<int> point,
                  std::unordered_set<aoc::point<int>>& points,
                  const aoc::matrix<int>& grid) {
    if (points.count(point) == 0 || grid(point) >= 9) {
        return;
    }
    points.erase(point);
    for (auto offset : std::initializer_list<aoc::point<int>>{{+0, +1},
                                                              {+0, -1},
                                                              {+1, +0},
                                                              {-1, +0}}) {
        const auto test_point = point + offset;
        remove_basin(test_point, points, grid);
    }
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    auto grid = parse(AOC_FWD(input));

    std::unordered_set<aoc::point<int>> points;
    for (ptrdiff_t x = 1; x < grid.x() - 1; ++x) {
        for (ptrdiff_t y = 1; y < grid.y() - 1; ++y) {
            if (grid(x, y) < 9) {
                points.emplace(x, y);
            }
        }
    }

    std::vector<size_t> basin_sizes;
    while (!points.empty()) {
        const auto start = points.size();
        remove_basin(*points.begin(), points, grid);
        basin_sizes.push_back(start - points.size());
    }

    std::sort(basin_sizes.begin(), basin_sizes.end(), std::greater<>{});
    return basin_sizes[0] * basin_sizes[1] * basin_sizes[2];
}

int main() { aoc::dbg(solution(aoc::input_range<aoc::line>())); }
