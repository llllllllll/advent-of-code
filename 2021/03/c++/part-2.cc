#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
auto solution(R&& input) {
    constexpr size_t max_bits = 64;
    using BitSet = std::bitset<max_bits>;

    size_t bits;
    std::vector<BitSet> parsed;
    for (const auto& line : input) {
        bits = line.size();
        parsed.emplace_back(BitSet{aoc::parse<uint64_t>(line, 2)});
    }

    auto most_common = [](const auto& range, size_t ix) {
        size_t trues = 0;
        size_t falses = 0;

        for (const auto& bin : range) {
            trues += bin[ix];
            falses += !bin[ix];
        }
        return trues >= falses;
    };

    auto drop_bit = [](std::unordered_set<BitSet>& inp, size_t ix, bool bit) {
        for (auto it = inp.begin(); it != inp.end();) {
            if ((*it)[ix] != bit) {
                it = inp.erase(it);
            }
            else {
                ++it;
            }
        }
    };

    auto find = [&](auto&& cond) {
        std::unordered_set<BitSet> ratings(parsed.begin(), parsed.end());
        for (ptrdiff_t ix = bits - 1; ix >= 0; --ix) {
            const auto bit = cond(most_common(ratings, ix));
            drop_bit(ratings, ix, bit);
            if (ratings.size() == 1) {
                break;
            }
        }
        return *ratings.begin();
    };

    const auto oxygen = find(std::identity{});
    const auto co2 = find(std::logical_not<>{});
    return oxygen.to_ulong() * co2.to_ulong();
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
