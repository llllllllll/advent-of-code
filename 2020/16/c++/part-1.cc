#include "aoc/aoc.hpp"

struct range {
    std::array<int, 2> a;
    std::array<int, 2> b;

    bool operator()(int v) const {
        return (a[0] <= v && v <= a[1]) || (b[0] <= v && v <= b[1]);
    }
};

template<std::ranges::range R>
std::unordered_map<std::string, range> parse_fields(R&& input) {
    std::unordered_map<std::string, range> out;
    for (auto sv : input) {
        auto res = aoc::regex_match("(.*): ([0-9]+)-([0-9]+) or ([0-9]+)-([0-9]+)", sv);
        if (!res) {
            throw aoc::bad_parse<range>(sv);
        }
        out.emplace((*res)[1],
                    range{{aoc::parse<int>((*res)[2].str()),
                           aoc::parse<int>((*res)[3].str())},
                          {aoc::parse<int>((*res)[4].str()),
                           aoc::parse<int>((*res)[5].str())}});
    }
    return out;
}

std::vector<int> parse_ticket(std::string_view sv) {
    auto fs = aoc::split(sv, ',');
    auto it = fs | std::views::transform(aoc::parse<int>);
    return {it.begin(), it.end()};
}

template<std::ranges::range R>
auto solution(R&& input) {
    const auto fields = parse_fields(
        input | std::views::take_while([](auto sv) { return !sv.empty(); }));
    const auto my_ticket = parse_ticket(*(input | std::views::drop_while([](auto sv) {
                                              return sv != "your ticket:";
                                          }) |
                                          std::views::drop(1))
                                             .begin());

    int scanning_error_rate = 0;
    for (auto line : input | std::views::drop_while([](auto sv) {
                         return sv != "nearby tickets:";
                     }) | std::views::drop(1)) {
        for (int field : parse_ticket(line)) {
            if (std::ranges::none_of(fields | std::views::values,
                                     [field](auto f) { return f(field); })) {
                scanning_error_rate += field;
            }
        }
    }
    return scanning_error_rate;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
