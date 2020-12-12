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

private:
    static constexpr coord north{0, -1};
    static constexpr coord north_east{+1, -1};
    static constexpr coord east{+1, 0};
    static constexpr coord south_east{+1, +1};
    static constexpr coord south{0, +1};
    static constexpr coord south_west{-1, +1};
    static constexpr coord west{-1, 0};
    static constexpr coord north_west{-1, -1};
    static constexpr std::array directions =
        {north, north_east, east, south_east, south, south_west, west, north_west};

    std::array<space, directions.size()> line_of_sight(coord c) const {

        auto make_slope_iterator = [&](coord slope) {
            return std::ranges::subrange{const_slope_iterator{slope.x, slope.y, c, this},
                                         const_slope_iterator::end_sentinel{x_, y_}};
        };

        std::array<space, directions.size()> out;
        for (std::size_t ix = 0; ix < directions.size(); ++ix) {
            const auto r = make_slope_iterator(directions[ix]);
            out[ix] = *std::ranges::find_if(++r.begin(), r.end(), [](auto s) {
                return s != space::floor;
            });
        }
        return out;
    }

public:
    room step() const {
        room out = *this;
        for (std::ptrdiff_t y = 1; y < y_ - 1; ++y) {
            for (std::ptrdiff_t x = 1; x < x_ - 1; ++x) {
                switch (space& s = out[{x, y}]) {
                case space::floor:
                    break;
                case space::unoccupied: {
                    const auto los = line_of_sight({x, y});
                    if (std::none_of(los.begin(), los.end(), [](auto s) {
                            return s == space::occupied;
                        })) {
                        s = space::occupied;
                    }
                    break;
                }
                case space::occupied: {
                    const auto los = line_of_sight({x, y});
                    if (std::count(los.begin(), los.end(), space::occupied) >= 5) {
                        s = space::unoccupied;
                    }
                    break;
                }
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

    class const_slope_iterator {
    public:
        using value_type = const space;
        using reference_type = const space&;
        using difference_type = std::ptrdiff_t;

        const_slope_iterator() = default;
        const_slope_iterator(const const_slope_iterator&) = default;
        const_slope_iterator& operator=(const const_slope_iterator&) = default;

        const_slope_iterator& operator++() {
            coord_.x += x_;
            coord_.y += y_;
            return *this;
        }

        const_slope_iterator operator++(int) {
            const_slope_iterator out = *this;
            ++*this;
            return out;
        }

        reference_type operator*() const {
            return (*room_)[coord_];
        }

        auto operator<=>(const const_slope_iterator& other) const = default;

        struct end_sentinel {
            std::ptrdiff_t end_x;
            std::ptrdiff_t end_y;
        };

    private:
        friend room;

        friend bool operator==(const const_slope_iterator& a, end_sentinel b) {
            auto out = a.coord_.x == 0 || a.coord_.y == 0 || a.coord_.x == b.end_x - 1 ||
                       a.coord_.y == b.end_y - 1;
            return out;
        }

        friend bool operator!=(const const_slope_iterator& a, end_sentinel b) {
            return !(a == b);
        }

        const_slope_iterator(std::ptrdiff_t x, std::ptrdiff_t y, coord c, const room* r)
            : x_(x), y_(y), coord_(c), room_(r) {}

        std::ptrdiff_t x_;
        std::ptrdiff_t y_;
        coord coord_;
        const room* room_;
    };
    static_assert(
        std::sentinel_for<const_slope_iterator::end_sentinel, const_slope_iterator>);

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
        std::cout << r << '\n';
    }
}

int main() {
    aoc::dbg(solution(aoc::input_range<std::string>()));
}
