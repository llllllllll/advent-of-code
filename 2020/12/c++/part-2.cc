#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <string_view>
#include <bit>

#include "aoc/aoc.hpp"

enum class instr {
    n,
    e,
    s,
    w,
    f,
    l,
    r,
};

instr parse_instr(char c) {
    switch (c) {
    case 'N':
        return instr::n;
    case 'E':
        return instr::e;
    case 'S':
        return instr::s;
    case 'W':
        return instr::w;
    case 'F':
        return instr::f;
    case 'L':
        return instr::l;
    case 'R':
        return instr::r;
    }
    throw aoc::bad_parse<instr>{std::string_view{&c, 1}};
};

enum class direction {
    n,
    e,
    s,
    w,
};

direction instr_to_direction(instr i) {
    direction d;
    static_assert(sizeof(d) == sizeof(i));
    std::memcpy(&d, &i, sizeof(d));
    return d;
}

struct coord {
    std::ptrdiff_t x = 0;
    std::ptrdiff_t y = 0;
};

struct ship {
public:
    coord loc() const {
        return loc_;
    }

    void step(instr i, int val) {
        switch (i) {
        case instr::n ... instr::w:
            step_dir(instr_to_direction(i), val);
            break;
        case instr::f: {
            loc_.x += waypoint_.x * val;
            loc_.y += waypoint_.y * val;
            break;
        }
        case instr::l:
            rotate_waypoint(val);
            break;
        case instr::r:
            rotate_waypoint(360 - val);
        }
    }


private:
    void step_dir(direction d, int val) {
        switch (d) {
        case direction::n:
            waypoint_.y += val;
            break;
        case direction::s:
            waypoint_.y -= val;
            break;
        case direction::e:
            waypoint_.x += val;
            break;
        case direction::w:
            waypoint_.x -= val;
            break;
        }
    }

    void rotate_waypoint(int val) {
        constexpr std::array cos = {0, -1, 0, 1};
        constexpr std::array sin = {1, 0, -1, 0};

        int ix = val / 90 - 1;
        waypoint_ = {waypoint_.x * cos[ix] - waypoint_.y * sin[ix],
                     waypoint_.x * sin[ix] + waypoint_.y * cos[ix]};
    }

    coord loc_ = {0, 0};
    coord waypoint_ = {10, 1};
};

template<std::ranges::range R>
int answer(R&& input) {

    ship s;
    for (auto l : input) {
        std::string_view line{l};
        auto instr = parse_instr(l[0]);
        int val = aoc::parse<int>(line.substr(1));
        s.step(instr, val);
    }

    return std::abs(s.loc().x) + std::abs(s.loc().y);
}

int main() {
    aoc::dbg(answer(aoc::input_range<std::string>()));
}
