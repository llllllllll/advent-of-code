#include "aoc/aoc.hpp"

std::vector<std::string> parse(std::string_view line) {
    std::vector<std::string> out;

    aoc::for_split(line, ' ', [&](std::string_view cs) {
        if (cs != "|"sv) {
            out.emplace_back(cs);
        }
    });

    std::rotate(out.begin(), out.end() - 4, out.end());
    out.erase(out.begin() + 4, out.end());
    return out;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    std::vector<int> mapping = {-1, -1, 1, 7, 4, -1, -1, 8};
    size_t out = 0;
    for (const auto& line : input) {
        for (const auto& num : parse(line)) {
            out += mapping.at(num.size()) > 0;

        }
    }
    return out;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
