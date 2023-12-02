#include <experimental/simd>

#include "aoc/aoc.hpp"

struct Draws {
    int red = 0;
    int green = 0;
    int blue = 0;

    constexpr friend auto operator<=(Draws a, Draws b) -> bool {
        return a.red <= b.red and  a.green <= b.green and a.blue <= b.blue;
    }
};

struct Game {
    int id;
    Draws draws;
};

auto parse_game(std::string_view line) -> Game {
    auto parts = aoc::split(line, ':');
    AOC_ASSERT(parts.size() == 2);
    Game game;
    auto& game_id_str = parts.front();
    game.id = aoc::parse<int>(game_id_str.substr(game_id_str.find(' ') + 1));
    aoc::for_each_regex_match(R"((\d+) (red|green|blue))", parts.back(), [&](auto m) {
        int const count = aoc::parse<int>(m[1].str());
        auto max = [&](int& member) {
            member = std::max(member, count);
        };
        switch (m[2].str().front()) {
        case 'r': max(game.draws.red); break;
        case 'g': max(game.draws.green); break;
        case 'b': max(game.draws.blue); break;
        default: AOC_ASSERT(false);
        }
    });
    return game;
}

template <std::ranges::input_range R>
auto solution(R&& input) {
    constexpr Draws limit = {12, 13, 14};
    return std::ranges::fold_left(
        input
        | std::views::transform(parse_game)
        | std::views::filter([](auto game) { return game.draws <= limit; })
        | std::views::transform([](auto game) { return game.id; }),
        0,
        std::plus<>{});
}

auto main() -> int {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
