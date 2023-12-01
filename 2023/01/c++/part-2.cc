#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
auto solution(R&& input) {
    constexpr auto digits = "0123456789"sv;
    constexpr std::array words = {
        "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
    };
    int sum = 0;
    for (auto line : input) {
        size_t first_ix = line.find_first_of(digits);
        int first = -1;
        if (first_ix != line.npos) {
            first = line.at(first_ix) - '0';
        }
        for (auto [value, word] : ranges::views::enumerate(words)) {
            auto const ix = line.find(word);
            if (ix < first_ix) {
                first_ix = ix;
                first = value;
            }
        }

        size_t last_ix = 0;
        int last = -1;
        if (auto ix = line.find_last_of(digits); ix != line.npos) {
            last_ix = ix;
            last = line.at(last_ix) - '0';
        }
        for (auto [value, word] : ranges::views::enumerate(words)) {
            auto const ix = line.rfind(word);
            if (ix != line.npos and ix > last_ix) {
                last_ix = ix;
                last = value;
            }
        }
        sum += first * 10 + last;
    }
    return sum;
}

auto main() -> int {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
