#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
auto solution(R&& input) {
    auto const input_vec = input | ranges::to<std::vector>;
    aoc::matrix<int> matrix(input_vec[0].size(), input_vec.size());
    for (auto const& [x, line] : ranges::views::enumerate(input_vec)) {
        for (auto [y, c] : ranges::views::enumerate(line)) {
            matrix(x, y) = c - '0';
        }
    }

    using Point = aoc::point<ptrdiff_t>;
    std::unordered_set<Point> visible;
    auto scan = [&](Point p, Point delta) {
        visible.insert(p);
        int min = matrix(p);
        for (; matrix.contains_point(p); p += delta) {
            if (matrix(p) > min) {
                visible.insert(p);
                min = matrix(p);
            }
        }
    };

    for (ptrdiff_t x = 0; x < matrix.x(); ++x) {
        scan(Point(x, 0), Point(0, +1));
        scan(Point(x, matrix.y() - 1), Point(0, -1));
    }
    for (ptrdiff_t y = 0; y < matrix.y(); ++y) {
        scan(Point(0, y), Point(+1, 0));
        scan(Point(matrix.x() - 1, y), Point(-1, 0));
    }
    return visible.size();
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
