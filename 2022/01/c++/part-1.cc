#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
auto solution(R&& input) {
    int64_t max = 0;
    int64_t sum = 0;
    auto commit = [&] {
        if (sum > max) {
            max = sum;
        }
        sum = 0;
    };
    for (auto&& line : input) {
        if (line.get().empty()) {
            commit();
        }
        else {
            sum += aoc::parse<int64_t>(line.get());
        }
    }
    commit();
    return max;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
