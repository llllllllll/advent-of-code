#include "aoc/aoc.hpp"

int score(std::string_view cs) {
    constexpr auto closes = [] {
        std::array<char, 256> closes;
        closes[')'] = '(';
        closes[']'] = '[';
        closes['}'] = '{';
        closes['>'] = '<';
        return closes;
    }();

    constexpr auto scores = [] {
        std::array<int, 256> scores;
        scores[')'] = 3;
        scores[']'] = 57;
        scores['}'] = 1197;
        scores['>'] = 25137;
        return scores;
    }();

    std::vector<char> stack;
    for (char c : cs) {
        switch (c) {
        case '(':
        case '[':
        case '{':
        case '<':
            stack.push_back(c);
            break;
        case ')':
        case ']':
        case '}':
        case '>':
            if (stack.back() != closes[c]) {
                return scores[c];
            }
            stack.pop_back();
            break;
        default:
            aoc::no_solution();
        }
    }
    return 0;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    int sum = 0;
    for (auto score : input | std::views::transform(score)) {
        sum += score;
    }
    return sum;
}

int main() { aoc::dbg(solution(aoc::input_range<aoc::line>())); }
