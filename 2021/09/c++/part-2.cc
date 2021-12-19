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
    for (ptrdiff_t row = 1; row < grid.rows() - 1; ++row) {
        for (ptrdiff_t col = 1; col < grid.cols() - 1; ++col) {
            if (grid(row, col) < 9) {
                points.emplace(row, col);
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
