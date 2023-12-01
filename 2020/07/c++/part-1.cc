#include <array>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "aoc/line.hpp"

using contents = std::vector<std::pair<std::string, int>>;

struct rule {
    std::string outer;
    contents inner;

    static rule parse(std::string_view line) {
        rule out;
        auto outer_ix = line.find(" contain");
        out.outer = line.substr(0, outer_ix);
        auto rest = line.substr(outer_ix + std::strlen(" contain"));
        while (true) {
            rest.remove_prefix(1);
            if (rest == "no other bags.") {
                break;
            }
            else {
                char* end;
                int count = std::strtol(rest.data(), &end, 10);
                rest.remove_prefix(end - rest.data() + 1);
                auto end_ix = rest.find(',');
                if (end_ix == std::string_view::npos) {
                    rest.remove_suffix(1);
                    out.inner.emplace_back(rest, count);
                    if (count == 1) {
                        out.inner.back().first.push_back('s');
                    }
                    break;
                }
                out.inner.emplace_back(rest.substr(0, end_ix), count);
                if (count == 1) {
                    out.inner.back().first.push_back('s');
                }
                rest.remove_prefix(end_ix);
            }
        }
        return out;
    }
};

template<typename I>
auto parse(I begin, I end) {
    std::unordered_map<std::string, std::vector<std::string>> graph;
    for (I it = begin; it != end; ++it) {
        auto r = rule::parse(*it);
        for (const auto& [inner, _] : r.inner) {
            graph[inner].emplace_back(r.outer);
        }
    }
    return graph;
}

void count(const std::string& start,
           const std::unordered_map<std::string, std::vector<std::string>>& graph,
           std::unordered_set<std::string>& seen) {
    auto search = graph.find(start);
    if (search == graph.end()) {
        return;
    }
    for (const auto& next : search->second) {
        seen.emplace(next);
        count(next, graph, seen);
    }
}

int main() {
    auto graph = parse(std::istream_iterator<aoc::line>{std::cin},
                       std::istream_iterator<aoc::line>{});

    std::unordered_set<std::string> seen;
    count("shiny gold bags", graph, seen);
    std::cout << seen.size() << '\n';
}
