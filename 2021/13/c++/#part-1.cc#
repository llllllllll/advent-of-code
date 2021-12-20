#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
auto solution(R&& input) {
    auto it = input.begin();
    const auto end = input.end();
    std::unordered_set<aoc::point<>> points;
    for (; it != end; ++it) {
        const auto& line = *it;
        if (line.get() == ""sv) {
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
            if (axis == 'x' && point.x() < cut) {
                new_points.emplace(cut - (point.x() - cut), point.y());
            }
            else if (axis == 'y' && point.y() < cut) {
                new_points.emplace(point.x(), cut - (point.y() - cut));
            }
            else {
                new_points.emplace(point);
            }
        }
        swap(points, new_points);
        break;
    }
    return points.size();
}

int main() { aoc::dbg(solution(aoc::input_range<aoc::line>())); }
