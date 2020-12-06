#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

#include <utils/line.hpp>

using answers = std::array<bool, 26>;

answers parse_answers(std::string_view line) {
    answers out = {false};
    for (char c : line) {
        out[c - 'a'] = true;
    }
    return out;
}

template<typename I>
std::vector<std::vector<answers>> parse_groups(I begin, I end) {
    std::vector<std::vector<answers>> groups;
    groups.emplace_back();
    for (I it = begin; it != end; ++it) {
        const std::string& line = *it;
        if (line.empty()) {
            groups.emplace_back();
            continue;
        }
        groups.back().emplace_back(parse_answers(line));
    }
    return groups;
}

answers merge_answers(const std::vector<answers>& group) {
    answers out = {false};
    for (const answers& ans : group) {
        for (std::size_t ix = 0; ix < out.size(); ++ix) {
            out[ix] |= ans[ix];
        }
    }
    return out;
}

template<typename I>
int answer(I begin, I end) {
    const auto groups = parse_groups(begin, end);
    std::size_t sum = 0;
    for (const auto& group : groups) {
        for (bool b : merge_answers(group)) {
            sum += b;
        }
    }
    return sum;
}

int main() {
    std::cout << answer(std::istream_iterator<aoc::line>{std::cin},
                        std::istream_iterator<aoc::line>{})
              << '\n';
}
