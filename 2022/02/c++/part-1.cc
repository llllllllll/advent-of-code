#include <compare>

#include "aoc/aoc.hpp"

enum class Throw : uint8_t {
    rock,
    paper,
    scissors,
};

enum class Opponent : char {
    rock = 'A',
    paper = 'B',
    scissors = 'C',
};

enum class Response : char {
    rock = 'X',
    paper = 'Y',
    scissors = 'Z',
};

enum class Result : int {
    loss = 0,
    tie = 3,
    win = 6,
};

auto result(Opponent a, Response b) -> Result {
    switch (a) {
    case Opponent::rock:
        switch (b) {
        case Response::rock: return Result::tie;
        case Response::paper: return Result::win;
        case Response::scissors: return Result::loss;
        default: __builtin_unreachable();
        }
    case Opponent::paper:
        switch (b) {
        case Response::rock: return Result::loss;
        case Response::paper: return Result::tie;
        case Response::scissors: return Result::win;
        default: __builtin_unreachable();
        }
    case Opponent::scissors:
        switch (b) {
        case Response::rock: return Result::win;
        case Response::paper: return Result::loss;
        case Response::scissors: return Result::tie;
        default: __builtin_unreachable();
        }
    default: __builtin_unreachable();
    }
}

auto score(Opponent a, Response b) -> int {
    return (static_cast<char>(b) - static_cast<char>(Response::rock) + 1)
        + static_cast<int>(result(a, b));
}


template<std::ranges::input_range R>
auto solution(R&& input) {
    int total_score = 0;
    for (auto&& line : input) {
        Opponent const a{line[0]};
        Response const b{line[2]};
        total_score += score(a, b);
    }
    return total_score;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
