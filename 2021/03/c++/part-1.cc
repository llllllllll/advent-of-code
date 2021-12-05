#include "aoc/aoc.hpp"

template<std::ranges::input_range R>
auto solution(R&& input) {
    constexpr size_t max_bits = 64;
    using BitSet = std::bitset<max_bits>;

    std::vector<size_t> true_counts(max_bits);
    std::vector<size_t> false_counts(max_bits);
    size_t bits;
    for (auto line : input) {
        bits = line.get().size();
        const auto bin = BitSet{aoc::parse<uint64_t>(line, 2)};
        for (size_t ix = 0; ix < bits; ++ix) {
            (bin[ix] ? true_counts : false_counts)[ix] += 1;
        }
    }
    BitSet gamma;
    BitSet epsilon;
    for (size_t ix = 0; ix < bits; ++ix) {
        if (true_counts[ix] > false_counts[ix]) {
            gamma[ix] = true;
        }
        else {
            epsilon[ix] = true;
        }
    }
    return gamma.to_ulong() * epsilon.to_ulong();
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
