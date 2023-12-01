#include "aoc/aoc.hpp"

struct point : public std::array<int, 3> {
    std::size_t hash() const {
        return aoc::hash_many((*this)[0], (*this)[1], (*this)[2]);
    }
};

auto cube_range(point p, int n = 3) {
    if (!(n & 1)) {
        throw std::runtime_error{"cube_range n must be odd"};
    }
    const int offset = n / 2;
    return std::views::iota(0, n * n * n) | std::views::transform([p, n, offset](int a) {
               auto x = a % n;
               a /= n;
               auto y = a % n;
               a /= n;
               return point{x - offset + p[0], y - offset + p[1], a - offset + p[2]};
           });
}

using grid = std::unordered_set<point, aoc::hasher>;

template<std::ranges::range R>
grid parse(R&& input) {
    grid out;
    int y = 0;
    for (auto line : input) {
        int x = 0;
        for (char c : line) {
            if (c == '#') {
                out.emplace(point{x, y, 0});
            }
            ++x;
        }
        ++y;
    }
    if (!(y & 1)) {
        ++y;
    }
    return out;
}

grid update(const grid& g) {
    grid out;
    auto check = [&g, &out](point p) {
        auto cr = cube_range(p);
        const auto active =
            std::accumulate(cr.begin(), cr.end(), 0, [&g](auto acc, auto q) {
                return acc + g.count(q);
            });
        if (g.count(p)) {
            if (active == 3 || active == 4) {
                out.emplace(p);
            }
        }
        else {
            if (active == 3) {
                out.emplace(p);
            }
        }
    };
    for (point p : g) {
        for (auto q : cube_range(p)) {
            check(q);
        }
    }
    return out;
}

template<std::ranges::range R>
auto solution(R&& input) {
    constexpr std::size_t max_step = 6;

    grid g = parse(input);
    for (std::size_t step = 0; step < max_step; ++step) {
        g = update(g);
    }
    return g.size();
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
