#include "aoc/aoc.hpp"

long score(std::string_view cs) {
    constexpr auto closes = [] {
        std::array<char, 256> closes;
        closes[')'] = '(';
        closes[']'] = '[';
        closes['}'] = '{';
        closes['>'] = '<';
        return closes;
    }();

    constexpr auto scores = [] {
        std::array<long, 256> scores;
        scores['('] = 1;
        scores['['] = 2;
        scores['{'] = 3;
        scores['<'] = 4;
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
                return std::numeric_limits<long>::max();
            }
            stack.pop_back();
            break;
        default:
            aoc::no_solution();
        }
    }

    long score = 0;
    for (char c : stack | std::views::reverse) {
        score *= 5;
        score += scores[c];
    }
    return score;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    std::vector<long> scores;
    for (auto score : input | std::views::transform(score)) {
        if (score != std::numeric_limits<long>::max()) {
            scores.push_back(score);
        }
    }
    std::sort(scores.begin(), scores.end());
    return scores[scores.size() / 2];
}

int main() { aoc::dbg(solution(aoc::input_range<aoc::line>())); }
