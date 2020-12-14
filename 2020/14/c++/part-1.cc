#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <regex>

#include "aoc/aoc.hpp"

struct mask {
    std::uint64_t trues = 0;
    std::uint64_t falses = 0;

    std::uint64_t operator()(std::uint64_t val) const {
        val |= trues;
        val &= ~falses;
        return val;
    }
};

class machine {
public:
    void set_mem(std::intptr_t addr, std::uint64_t val) {
        mem_[addr] = mask_(val);
    }

    void set_mask(mask m) {
        mask_ = m;
    }

    const std::unordered_map<std::intptr_t, std::uint64_t>& mem() const {
        return mem_;
    }

private:
    mask mask_;
    std::unordered_map<std::intptr_t, std::uint64_t> mem_;
};

mask parse_mask(std::string_view v) {
    mask out;
    int ix = 0;
    for (char c : v | std::views::reverse) {
        switch (c) {
        case 'X':
            break;
        case '1':
            out.trues |= 1ul << ix;
            break;
        case '0':
            out.falses |= 1ul << ix;
            break;
        default:
            return out;
        }
        ++ix;
    }
    __builtin_unreachable();
}

template<std::ranges::range R>
auto solution(R&& input) {
    machine m;
    for (const auto& l : input) {
        if (aoc::starts_with(l, "mask = ")) {
            m.set_mask(parse_mask(l));
            continue;
        }

        const std::regex mem_regex{R"(mem\[([0-9]+)\] = ([0-9]+))"};

        std::smatch match;
        if (!std::regex_match(l.get(), match, mem_regex)) {
            throw aoc::bad_parse<std::string>(l);
        }
        m.set_mem(aoc::parse<std::intptr_t>(match[1].str()),
                  aoc::parse<std::uint64_t>(match[2].str()));
    }

    return std::accumulate(m.mem().begin(), m.mem().end(), 0ul, [](auto a, auto b) {
        return a + b.second;
    });
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
