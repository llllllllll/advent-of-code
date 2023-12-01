#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
void solution(R&& input) {
    auto it = input.begin();
    const auto end = input.end();
    std::unordered_set<aoc::point<>> points;
    for (; it != end; ++it) {
        const auto& line = *it;
        if (line == ""sv) {
            ++it;
            break;
        }
        if (auto m = aoc::regex_match("([0-9]+),([0-9]+)", line)) {
            points.emplace(aoc::parse<ptrdiff_t>((*m)[1].str()),
                           aoc::parse<ptrdiff_t>((*m)[2].str()));
        }
        else {
            throw aoc::bad_parse<void>(line);
        }
    }

    std::vector<std::pair<char, int>> folds;
    for (; it != end; ++it) {
        const auto& line = *it;
        if (auto m = aoc::regex_match("fold along (x|y)=([0-9]+)", line)) {
            folds.emplace_back((*m)[1].str()[0], aoc::parse<int>((*m)[2].str()));
        }
        else {
            throw aoc::bad_parse<void>(line);
        }
    }

    for (auto [axis, cut] : folds) {
        std::unordered_set<aoc::point<>> new_points;
        for (const auto& point : points) {
            if (axis == 'x' && point.x() > cut) {
                new_points.emplace(cut - (point.x() - cut), point.y());
            }
            else if (axis == 'y' && point.y() > cut) {
                new_points.emplace(point.x(), cut - (point.y() - cut));
            }
            else {
                new_points.emplace(point);
            }
        }
        swap(points, new_points);
    }

    ptrdiff_t min_x = std::numeric_limits<ptrdiff_t>::max();
    ptrdiff_t min_y = std::numeric_limits<ptrdiff_t>::max();
    ptrdiff_t max_x = 0;
    ptrdiff_t max_y = 0;
    for (const auto& p : points) {
        min_x = std::min(min_x, p.x());
        min_y = std::min(min_y, p.y());
        max_x = std::max(max_x, p.x());
        max_y = std::max(max_y, p.y());
    }
    aoc::point<> offset{min_x, min_y};
    aoc::matrix<char> mat(max_x - min_x + 1, max_y - min_y + 1, '.');
    for (const auto& p : points) {
        mat(p - offset) = '#';
    }
    auto T = mat.transpose();
    for (ptrdiff_t x = 0; x < T.x(); ++x) {
        for (ptrdiff_t y = 0; y < T.y(); ++y) {
            std::cout << T(x, y);
        }
        std::cout << '\n';
    }
}

int main() { solution(aoc::input_range<aoc::line>()); }
