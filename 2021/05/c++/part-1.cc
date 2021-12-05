#include "aoc/aoc.hpp"

using point = std::complex<int>;

class line_segment {
public:
    line_segment(point start, point end)
        : start_{start}
        , end_{end} {}

    point start() const { return start_; }
    point end() const { return end_; }

    static line_segment parse(std::string_view line) {
        if (auto match = aoc::regex_match("([0-9]+),([0-9]+) -> ([0-9]+),([0-9]+)", line)) {
            return line_segment{
                {aoc::parse<int>((*match)[1].str()),
                 aoc::parse<int>((*match)[2].str())},
                {aoc::parse<int>((*match)[3].str()),
                 aoc::parse<int>((*match)[4].str())}};
        }
        throw aoc::bad_parse<line_segment>(line);
    }

    bool is_vertical() const {
        return start_.real() == end_.real() || start_.imag() == end_.imag();
    }

    template<typename F>
    void for_each_point(F&& f) {
        point incr;
        if (start_.real() > end_.real()) {
            incr.real(-1);
        }
        else if (start_.real() < end_.real()) {
            incr.real(1);
        }

        if (start_.imag() > end_.imag()) {
            incr.imag(-1);
        }
        else if (start_.imag() < end_.imag()) {
            incr.imag(1);
        }
        const auto end = end_ + incr;
        for (point it = start_; it != end; it += incr) {
            std::invoke(f, it);
        }
    }

private:
    friend std::ostream& operator<<(std::ostream& os, const line_segment& ls) {
        return os << ls.start_.real() << ',' << ls.start_.imag() << " -> "
                  << ls.end_.real() << ',' << ls.end_.imag();
    }

    point start_;
    point end_;
};

namespace std {
template <>
struct hash<point> {
    size_t operator()(point p) const {
        return aoc::hash_many(p.real(), p.imag());
    }
};
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    std::unordered_map<point, size_t, aoc::hasher> points;

    for (auto line : input | std::views::transform(line_segment::parse)) {
        if (line.is_vertical()) {
            line.for_each_point([&](point p) {
                ++points[p];
            });
        }
    }
    return std::accumulate(points.begin(), points.end(), 0, [](auto a, auto b) {
        return a + (b.second >= 2);
    });
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
