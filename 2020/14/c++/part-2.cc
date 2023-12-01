#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <numeric>
#include <regex>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "aoc/aoc.hpp"

class mask {
public:
    mask(std::uint64_t trues, std::uint64_t floating)
        : trues_(trues),
          floating_(floating) {
        fill(floating, 0);
    }

    template<typename F>
    void operator()(std::uint64_t val, F&& f) const {
        val |= trues_;
        val &= ~floating_;
        for (std::uint64_t m : floating_alts_) {
            f(val | m);
        }
    }

private:
    void fill(std::uint64_t floating, std::uint64_t mask) {
        if (floating == 0) {
            floating_alts_.emplace_back(mask);
            return;
        }

        const auto lz = __builtin_clzl(floating | 1);
        const int bit = sizeof(floating) * 8 - lz - 1;
        const std::uint64_t new_floating = floating & ~(1ul << bit);
        fill(new_floating, mask | 1ul << bit);
        fill(new_floating, mask);
    }


    std::uint64_t trues_ = 0;
    std::uint64_t floating_ = 0;
    std::vector<std::uint64_t> floating_alts_;
};

class machine {
public:
    void set_mem(std::intptr_t addr, std::uint64_t val) {
        mask_(addr, [this, val](std::uint64_t addr) { mem_[addr] = val; });
    }

    void set_mask(mask m) {
        mask_ = m;
    }

    const std::unordered_map<std::intptr_t, std::uint64_t>& mem() const {
        return mem_;
    }

private:
    mask mask_{0, 0};
    std::unordered_map<std::intptr_t, std::uint64_t> mem_;
};

mask parse_mask(std::string_view v) {
    std::uint64_t trues = 0;
    std::uint64_t floating = 0;
    int ix = 0;
    for (char c : v | std::views::reverse) {
        switch (c) {
        case 'X':
            floating |= 1ul << ix;
            break;
        case '1':
            trues |= 1ul << ix;
            break;
        case '0':
            break;
        default:
            return mask{trues, floating};
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
        if (!std::regex_match(l, match, mem_regex)) {
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
