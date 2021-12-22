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

class memo_table {
private:
    struct entry {
        char first;
        char second;
        size_t step;

        bool operator==(const entry&) const = default;

        size_t hash() const noexcept { return first * second * step; }
    };

public:
    auto try_emplace(char first, char second, size_t step) {
        return storage_.try_emplace({first, second, step});
    }

private:
    struct hasher {
        size_t operator()(const entry& e) const noexcept { return e.hash(); }
    };

    std::unordered_map<entry, std::array<size_t, 26>, hasher> storage_;
};

template<std::ranges::input_range R>
std::pair<std::string, translation_table> parse(R&& input) {
    std::pair<std::string, translation_table> out;

    auto it = input.begin();
    const auto end = input.end();

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

std::array<size_t, 26>
search(char a, char b, const translation_table& trans, memo_table& m, size_t step) {
    auto [it, inserted] = m.try_emplace(a, b, step);
    if (!inserted) {
        return it->second;
    }
    std::array<size_t, 26> out = {0};
    if (step == 0) {
        return out;
    }
    if (auto c = trans(a, b)) {
        ++out[c - 'A'];
        const auto ac = search(a, c, trans, m, step - 1);
        const auto cb = search(c, b, trans, m, step - 1);
        for (size_t ix = 0; ix < 26; ++ix) {
            out[ix] += ac[ix] + cb[ix];
        }
    }
    return it->second = out;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    const auto [base, trans] = parse(AOC_FWD(input));

    constexpr size_t steps = 40;
    memo_table m;
    std::array<size_t, 26> out = {0};
    for (char c : base) {
        ++out[c - 'A'];
    }
    for (size_t ix = 0; ix < base.size() - 1; ++ix) {
        auto res = search(base[ix], base[ix + 1], trans, m, steps);
        for (size_t ix = 0; ix < 26; ++ix) {
            out[ix] += res[ix];
        }
    }

    size_t min = std::numeric_limits<size_t>::max();
    size_t max = 0;
    for (size_t count : out) {
        if (count > 0) {
            min = std::min(min, count);
            max = std::max(max, count);
        }
    }
    return max - min;
}

int main() { aoc::dbg(solution(aoc::input_range<aoc::line>())); }
