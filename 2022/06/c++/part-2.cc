#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
auto solution(R&& input) {
    constexpr size_t window_size = 14;
    size_t ix = window_size;
    auto sliding = ranges::views::sliding(input, ix);
    size_t unique_count = 0;
    std::array<size_t, 26> counts = {0};
    for (char const c : sliding.front()) {
        unique_count += counts[c - 'a']++ == 0;
    }
    if (unique_count == 4) {
        return ix;
    }
    char to_drop = input[0];
    for (auto window : sliding | std::views::drop(1)) {
        ++ix;

        unique_count -= --counts[to_drop - 'a'] == 0;
        to_drop = window.front();

        unique_count += counts[window.back() - 'a']++ == 0;
        if (unique_count == window_size) {
            return ix;
        }
    }
    AOC_ASSERT(false);
}

int main() {
    aoc::dbg(solution(aoc::all_input()));
}
