#include "aoc/aoc.hpp"

constexpr auto is_digit(char c) -> bool {
    return '0' <= c and c <= '9';
}

template <typename F>
auto adjacent_visit(aoc::matrix<char> const& m, aoc::point<> p, F&& f) {
    for (ptrdiff_t x : {-1, 0, +1}) {
        for (ptrdiff_t y : {-1, 0, +1}) {
            auto const candidate = p + aoc::point<>{x, y};
            if ((x == 0 and y == 0) or not m.contains_point(candidate)) {
                continue;
            }
            f(candidate, m(candidate));
        }
    }
}

template <std::ranges::input_range R>
auto solution(R&& input) {
    auto matrix = aoc::matrix<char>(input);
    std::unordered_map<aoc::point<>, std::vector<int>> gear_candidates;

    ptrdiff_t row = 0;
    for (auto&& r : matrix.rows()) {
        auto it = r.begin();
        auto const end = r.end();
        ptrdiff_t col = 0;

        int number = 0;

        std::unordered_set<aoc::point<>> adjacent_gears;

        auto seal = [&] {
            for (auto p : adjacent_gears) {
                gear_candidates[p].push_back(number);
            }
            adjacent_gears.clear();
            number = 0;
        };
        while (it != end) {
            for (; it != end; ++it) {
                if (is_digit(*it)) {
                    number *= 10;
                    number += *it - '0';

                    adjacent_visit(matrix, {row, col}, [&](aoc::point<> p, char c) {
                        if (c == '*') {
                            adjacent_gears.insert(p);
                        }
                    });
                }
                else {
                    seal();
                }
                ++col;
            }
            seal();
        }
        ++row;
    }
    return std::ranges::fold_left(
        gear_candidates
            | std::views::values
            | std::views::filter([](auto const& cs) { return cs.size() == 2; })
            | std::views::transform([](auto const& cs) { return cs.front() * cs.back(); }),
        0,
        std::plus<>{});
}

auto main() -> int {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
