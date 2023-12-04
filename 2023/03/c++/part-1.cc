#include "aoc/aoc.hpp"

constexpr auto is_digit(char c) -> bool {
    return '0' <= c and c <= '9';
}

constexpr auto is_symbol(char c) -> bool {
    return not is_digit(c) and c != '.';
}

template <typename F>
auto adjacent_visit(aoc::matrix<char> const& m, aoc::point<> p, F&& f) {
    for (ptrdiff_t x : {-1, 0, +1}) {
        for (ptrdiff_t y : {-1, 0, +1}) {
            auto const candidate = p + aoc::point<>{x, y};
            if ((x == 0 and y == 0) or not m.contains_point(candidate)) {
                continue;
            }
            f(m(candidate));
        }
    }
}

template <std::ranges::input_range R>
auto solution(R&& input) {
    auto matrix = aoc::matrix<char>(input);

    int sum = 0;
    ptrdiff_t  row = 0;
    for (auto&& r : matrix.rows()) {
        auto it = r.begin();
        auto const end = r.end();
        ptrdiff_t col = 0;

        bool has_symbol = false;
        int number = 0;

        auto seal = [&] {
            sum += has_symbol * number;
            number = 0;
            has_symbol = false;
        };
        while (it != end) {
            for (; it != end; ++it) {
                if (is_digit(*it)) {
                    number *= 10;
                    number += *it - '0';

                    if (not has_symbol) {
                        adjacent_visit(matrix, {row, col}, [&](char c) {
                            has_symbol |= is_symbol(c);
                        });
                    }
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
    return sum;
}

auto main() -> int {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
