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
        case instr::f:
            step_dir(dir_, val);
            break;
        case instr::l:
            dir_ = static_cast<direction>((static_cast<int>(dir_) - val / 90) & 0b11);
            break;
        case instr::r:
            dir_ = static_cast<direction>((static_cast<int>(dir_) + val / 90) & 0b11);
            break;
        }
    }


private:
    void step_dir(direction d, int val) {
        switch (d) {
        case direction::n:
            loc_.y += val;
            break;
        case direction::s:
            loc_.y -= val;
            break;
        case direction::e:
            loc_.x += val;
            break;
        case direction::w:
            loc_.x -= val;
            break;
        }
    }

    coord loc_ = {0, 0};
    direction dir_ = direction::e;
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
