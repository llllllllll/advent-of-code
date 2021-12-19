#include "aoc/aoc.hpp"

int parse(std::string_view line) {
    std::array<std::array<int, 3>, 8> mapping = {{
        {-1, -1, -1},
        {-1, -1, -1},
        {1, -1, -1},
        {7, -1, -1},
        {4, -1, -1},
        {2, 3, 5},
        {0, 6, 9},
        {8, -1, -1},
    }};
    std::array<std::bitset<7>, 10> candidates;
    candidates.fill(-1);

    std::vector<std::string> sample;
    std::vector<std::string> data;
    std::vector<std::string>* ptr = &sample;
    aoc::for_split(line, ' ', [&](std::string_view cs) {
        if (cs == "|"sv) {
            ptr = &data;
        }
        ptr->emplace_back(cs);
    });

    for (const auto& cs : sample) {
        aoc::dbg(cs);
        const auto& valid = mapping[cs.size()];
        aoc::dbg(valid);
        for (int i = 0; static_cast<size_t>(i) < candidates.size(); ++i) {
            if (std::find(valid.begin(), valid.end(), i) == valid.end()) {
                for (char c : cs) {
                    aoc::dbg(c);
                    candidates[i].reset(c - 'a');
                    aoc::dbg(candidates[i]);
                }
            }
            aoc::dbg(candidates[i]);
        }
    }
    for (auto cand : candidates) {
        aoc::dbg(cand);
    }
    return 5;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    size_t out = 0;
    for (const auto& line : input) {
        parse(line);
    }
    return out;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
