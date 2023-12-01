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

    auto view_distance = [&](Point p, Point delta) {
        int distance = 0;
        int max = matrix(p);
        for (p += delta; matrix.contains_point(p); p += delta) {
            ++distance;
            if (matrix(p) >= max) {
                break;
            }
        }
        return distance;
    };
    auto view_score = [&](Point p) {
        return view_distance(p, {0, +1}) *
               view_distance(p, {0, -1}) *
               view_distance(p, {+1, 0}) *
               view_distance(p, {-1, 0});
    };

    int best_score = 0;
    for (ptrdiff_t x = 0; x < matrix.x(); ++x) {
        for (ptrdiff_t y = 0; y < matrix.y(); ++y) {
            best_score = std::max(best_score, view_score({x, y}));
        }
    }
    return best_score;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
