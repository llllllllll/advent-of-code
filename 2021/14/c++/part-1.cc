#include "aoc/aoc.hpp"

class translation_table {
public:
    char& operator()(char first, char second) {
        return storage_[first - 'A'][second - 'A'];
    }
    char operator()(char first, char second) const {
        return storage_[first - 'A'][second - 'A'];
    }

private:
    std::array<std::array<char, 26>, 26> storage_{{0}};
};

template<std::ranges::input_range R>
std::pair<std::string, translation_table> parse(R&& input) {
    std::pair<std::string, translation_table> out;

    auto it = input.begin();
    const auto end  = input.end();

    out.first = *it++;
    for (++it; it != end; ++it) {
        if (auto m = aoc::regex_match("([A-Z]{2}) -> ([A-Z])", *it)) {
            const auto input = (*m)[1].str();
            const auto output = (*m)[2].str();
            out.second(input[0], input[1]) = output[0];
        }
        else {
            throw aoc::bad_parse<void>(*it);
        }
    }

    return out;
}

std::string step(const std::string& base, const translation_table& trans) {
    std::string out;
    for (size_t ix = 0; ix < base.size() - 1; ++ix) {
        out.push_back(base[ix]);
        if (const char c = trans(base[ix], base[ix + 1])) {
            out.push_back(c);
        }
    }
    out.push_back(base.back());
    return out;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    const auto [base, trans] = parse(AOC_FWD(input));

    constexpr size_t steps = 10;

    std::string poly = base;
    for (size_t s = 0; s < steps; ++s) {
        poly = step(poly, trans);
    }

    std::array<size_t, 26> counts{{0}};
    for (char c : poly) {
        ++counts[c - 'A'];
    }
    size_t min = std::numeric_limits<size_t>::max();
    size_t max = 0;
    for (size_t count : counts) {
        if (count > 0) {
            min = std::min(min, count);
            max = std::max(max, count);
        }
    }
    return max - min;
}

int main() { aoc::dbg(solution(aoc::input_range<aoc::line>())); }
