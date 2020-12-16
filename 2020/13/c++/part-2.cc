#include <algorithm>
#include <cmath>
#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "aoc/aoc.hpp"

std::int64_t powmod(std::int64_t a, std::int64_t b, std::int64_t c) {
    a %= c;
    std::int64_t s = 1;
    while (b > 0) {
        if (b & 1) {
            s = (s * a) % c;
        }
        b /= 2;
        a = (a * a) % c;
    }
    return s;
}

template<std::ranges::range R>
long solution(R&& input) {
    struct bus {
        std::int64_t sched;
        std::int64_t offset;
    };

    std::vector<bus> buses;
    std::size_t offset = 0;
    for (const auto& s : aoc::split(*++std::ranges::begin(input), ',')) {
        if (s != "x") {
            buses.emplace_back(aoc::parse<std::int16_t>(s), offset);
        }
        ++offset;
    }

    std::uint64_t sched_prod =
        std::accumulate(buses.begin(), buses.end(), 1ll, [](auto acc, auto bus) {
            return acc * bus.sched;
        });
    return std::accumulate(buses.begin(), buses.end(), 0l, [&](auto acc, auto bus) {
        const std::int16_t a = bus.sched - (bus.offset % bus.sched);
        const std::int64_t b = sched_prod / bus.sched;
        const std::int64_t b_inv = powmod(b, bus.sched - 2, bus.sched);
        return (acc + ((a * b * b_inv) % sched_prod)) % sched_prod;
    });
}

int main() {
    aoc::dbg(solution(aoc::input_range<std::string>()));
}
