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

#include "aoc/aoc.hpp"

bool valid(int cand, const std::deque<long>& buf) {
    for (int x : buf) {
        for (int y : buf) {
            if (x != y && x + y == cand) {
                return true;
            }
        }
    }
    return false;
}

template<typename I>
std::vector<long> read_ints(I begin, I end) {
    std::vector<long> out;
    for (I it = begin; it != end; ++it) {
        out.emplace_back(aoc::parse<long>(it->get()));
    }
    return out;
}

template<typename I>
int first_invalid(I begin, I end) {
    std::deque<long> buf;
    for (I it = begin; it != end; ++it) {
        int cand = *it;
        if (buf.size() == 25) {
            if (!valid(cand, buf)) {
                return cand;
            }
            buf.pop_front();
        }
        buf.emplace_back(cand);
    }
    throw std::runtime_error{"no solution"};
}

template<typename I, typename M>
long find_weakness(long target, long sum, I begin, I end, M& memo) {
    const auto search = memo.find({begin, end});
    if (search != memo.end()) {
        return search->second;
    }
    if (sum == target) {
        return *std::min_element(begin, end) + *std::max_element(begin, end);
    }
    if (end - begin < 2 || sum < target) {
        return memo[{begin, end}] = -1;
    }
    const auto right = find_weakness(target, sum - *(end - 1), begin, end - 1, memo);
    if (right > 0) {
        return memo[{begin, end}] = right;
    }
    return memo[{begin, end}] = find_weakness(target, sum - *begin, begin + 1, end, memo);
}

int main() {
    const auto ints = read_ints(std::istream_iterator<aoc::line>{std::cin},
                                std::istream_iterator<aoc::line>{});
    const auto target = first_invalid(ints.begin(), ints.end());
    const auto sum = std::accumulate(ints.begin(), ints.end(), 0l);

    using I = decltype(ints.begin());
    struct hash {
        std::size_t operator()(std::pair<I, I> p) const {
            return reinterpret_cast<uintptr_t>(&*p.first) ^
                reinterpret_cast<uintptr_t>(&*p.second);
        }
    };

    std::unordered_map<std::pair<I, I>, long, hash> memo;
    std::cout << find_weakness(target, sum, ints.begin(), ints.end(), memo) << '\n';
}
