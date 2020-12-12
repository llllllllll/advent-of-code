#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <vector>

#include "aoc/aoc.hpp"

enum class space {
    floor = 0,
    unoccupied,
    occupied,
};

space parse_space(char c) {
    switch (c) {
    case 'L':
        return space::unoccupied;
    case '#':
        return space::occupied;
    case '.':
        return space::floor;
    }
    throw aoc::bad_parse<space>(std::string_view{&c, 1});
}

char to_char(space s) {
    switch (s) {
    case space::unoccupied:
        return 'L';
    case space::occupied:
        return '#';
    case space::floor:
        return '.';
    }
    throw std::runtime_error{"unknown space"};
}

class room {
public:
    struct coord {
        std::ptrdiff_t x;
        std::ptrdiff_t y;

        auto operator<=>(const coord&) const = default;
    };

    template<std::ranges::range R>
    static room parse(R&& r) {
        const auto begin = std::ranges::begin(r);
        const auto end = std::ranges::end(r);

        room out;
        if (begin == end) {
            return out;
        }
        out.x_ = begin->size() + 2;
        std::ptrdiff_t y = 0;
        for (auto it = begin; it != end; ++it) {
            out.spaces_.resize(out.x_ * (y + 2));
            std::ptrdiff_t x = 0;
            for (char c : *it) {
                out[{1 + x++, 1 + y}] = parse_space(c);
            }
            ++y;
        }
        out.y_ = y + 2;
        out.spaces_.resize(out.x_ * out.y_);
        return out;
    }

    space& operator[](coord c) {
        // return spaces_[c.x * x_ + c.y];
        return spaces_.at(x_ * c.y + c.x);
    }

    const space& operator[](coord c) const {
        // return spaces_[c.x * x_ + c.y];
        return spaces_.at(x_ * c.y + c.x);
    }

    room step() const {
        room out = *this;
        for (std::ptrdiff_t y = 1; y < y_ - 1; ++y) {
            for (std::ptrdiff_t x = 1; x < x_ - 1; ++x) {
                const auto box = box_around({x, y});
                switch (space& s = out[{x, y}]) {
                case space::floor:
                    break;
                case space::unoccupied:
                    if (std::none_of(box.begin(), box.end(), [](auto s) {
                            return s == space::occupied;
                        })) {
                        s = space::occupied;
                    }
                    break;
                case space::occupied:
                    if (std::count(box.begin(), box.end(), space::occupied) >= 5) {
                        s = space::unoccupied;
                    }
                    break;
                }
            }
        }
        return out;
    }

    auto operator<=>(const room&) const = default;

    auto begin() const {
        return const_box_iterator{1, x_ - 1, {1, 1}, this};
    }

    auto end() const {
        return const_box_iterator{1, x_ - 1, {1, y_}, this};
    }

private:
    class const_box_iterator {
    public:
        using value_type = const space;
        using reference_type = const space&;
        using difference_type = std::ptrdiff_t;

        const_box_iterator() = default;
        const_box_iterator(const const_box_iterator&) = default;
        const_box_iterator& operator=(const const_box_iterator&) = default;

        const_box_iterator& operator++() {
            if (coord_.x == x_end_) {
                ++coord_.y;
                coord_.x = x_start_;
            }
            else {
                ++coord_.x;
            }
            return *this;
        }

        const_box_iterator operator++(int) {
            const_box_iterator out = *this;
            ++*this;
            return out;
        }

        reference_type operator*() {
            return (*room_)[coord_];
        }

        difference_type operator-(const const_box_iterator& other) const {
            return x_end_ - coord_.x + coord_.y - other.coord_.y * (x_end_ - x_start_);
        }

        auto operator<=>(const const_box_iterator& other) const = default;

    private:
        friend room;

        const_box_iterator(std::ptrdiff_t x_start,
                           std::ptrdiff_t x_end,
                           coord c,
                           const room* r)
            : x_start_(x_start), x_end_(x_end), coord_(c), room_(r) {}

        std::ptrdiff_t x_start_;
        std::ptrdiff_t x_end_;
        coord coord_;
        const room* room_;
    };

    std::ranges::subrange<const_box_iterator> box_from(coord start, coord end) const {
        return {const_box_iterator{start.x, end.x, start, this},
                const_box_iterator{start.x, end.x, {start.x, end.y + 1}, this}};
    }

    std::ranges::subrange<const_box_iterator> box_around(coord c) const {
        return box_from({c.x - 1, c.y - 1}, {c.x + 1, c.y + 1});
    }

    friend std::ostream& operator<<(std::ostream& out, const room& r) {
        for (std::ptrdiff_t y = 1; y < r.y_ - 1; ++y) {
            for (std::ptrdiff_t x = 1; x < r.x_ - 1; ++x) {
                std::cout << to_char(r[{x, y}]);
            }
            std::cout << '\n';
        }
        return out;
    }

    std::ptrdiff_t x_ = 0;
    std::ptrdiff_t y_ = 0;
    std::vector<space> spaces_;
};

template<std::ranges::range R>
std::size_t solution(R&& input) {
    room r = room::parse(input);
    while (true) {
        room next = r.step();
        if (next == r) {
            return std::count(r.begin(), r.end(), space::occupied);
        }
        r = std::move(next);
    }
}

int main() {
    aoc::dbg(solution(aoc::input_range<std::string>()));
}
