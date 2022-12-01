#include <queue>
#include <vector>

#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
auto solution(R&& input) {
    constexpr size_t limit = 3;
    int64_t sum = 0;
    std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t>> top;
    auto commit = [&] {
        top.push(sum);
        if (top.size() > limit) {
            top.pop();
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
    if (sum) {
        commit();
    }

    for (size_t ix = 0; ix < limit; ++ix) {
        sum += top.top();
        top.pop();
    }
    return sum;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
