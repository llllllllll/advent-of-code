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

bool valid_ticket(const std::unordered_map<std::string, range>& fields,
                  const std::vector<int>& ticket) {
    for (int field : ticket) {
        if (std::ranges::none_of(fields | std::views::values,
                                 [field](auto f) { return f(field); })) {
            return false;
        }
    }
    return true;
}

template<std::ranges::range R>
auto solution(R&& input) {
    auto fields = parse_fields(
        input | std::views::take_while([](auto sv) { return !sv.empty(); }));
    const auto my_ticket = parse_ticket(*(input | std::views::drop_while([](auto sv) {
                                              return sv != "your ticket:";
                                          }) |
                                          std::views::drop(1))
                                             .begin());

    std::vector<std::vector<int>> tickets;
    for (auto ticket : input | std::views::drop_while([](std::string_view sv) {
                           return sv != "nearby tickets:";
                       }) | std::views::drop(1) |
                           std::views::transform(
                               [](std::string_view sv) { return parse_ticket(sv); })) {
        if (valid_ticket(fields, ticket)) {
            tickets.emplace_back(std::move(ticket));
        }
    }

    std::vector<std::unordered_set<std::string>> field_mapping_candidates(
        my_ticket.size());
    for (std::size_t ix = 0; ix < my_ticket.size(); ++ix) {
        for (const auto& [k, v] : fields) {
            if (std::all_of(tickets.begin(), tickets.end(), [&](const auto& t) {
                    return v(t[ix]);
                })) {
                field_mapping_candidates[ix].emplace(k);
            }
        }
    }

    std::unordered_map<std::string, std::size_t> field_mappings;
    while (field_mappings.size() != my_ticket.size()) {
        std::size_t ix = 0;
        for (auto& cs : field_mapping_candidates) {
            if (cs.size() == 1) {
                std::string k = *cs.begin();
                field_mappings[k] = ix;
                for (auto& cs : field_mapping_candidates) {
                    cs.erase(k);
                }
            }
            ++ix;
        }
    }

    std::size_t out = 1;
    for (const auto& [k, v] : field_mappings) {
        if (aoc::starts_with(k, "departure")) {
            out *= my_ticket[v];
        }
    }
    return out;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
