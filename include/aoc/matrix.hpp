#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <ranges>
#include <type_traits>
#include <vector>

#include <aoc/point.hpp>
#include <aoc/repr.hpp>

namespace aoc {
template<typename T>
class matrix {
public:
    matrix(ptrdiff_t x, ptrdiff_t y) : storage_(x * y), x_{x}, y_{y} {}

    matrix(ptrdiff_t x, ptrdiff_t y, const T& init)
        : storage_(x * y, init), x_{x}, y_{y} {}

    T& operator()(ptrdiff_t r, ptrdiff_t c) { return storage_[r * y_ + c]; }
    const T& operator()(ptrdiff_t r, ptrdiff_t c) const { return storage_[r * y_ + c]; }

    template<typename U>
    T& operator()(const point<U>& p) {
        return (*this)(p.x(), p.y());
    }
    template<typename U>
    const T& operator()(const point<U>& p) const {
        return (*this)(p.x(), p.y());
    }

    auto flat() const { return std::views::all(storage_); }
    auto flat() { return std::views::all(storage_); }

    ptrdiff_t x() const { return x_; }
    ptrdiff_t y() const { return y_; }

    template <typename F>
    void for_each(F&& f) {
        for (ptrdiff_t y = 0; y < this->y(); ++y) {
            for (ptrdiff_t x = 0; x < this->y(); ++y) {
                std::invoke(f, aoc::point<>{x, y}, (*this)(x, y));
            }
        }
    }
    template <typename F>
    void for_each(F&& f) const {
        for (ptrdiff_t y = 0; y < this->y(); ++y) {
            for (ptrdiff_t x = 0; x < this->y(); ++y) {
                std::invoke(f, aoc::point<>{x, y}, (*this)(x, y));
            }
        }
    }

    matrix transpose() const {
        matrix out{y(), x()};
        for_each([&](auto point, const auto& v) {
            out(point.y(), point.x()) = v;
        });
        return out;
    }

    template<typename U = T, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    point<> min() const {
        return min(std::numeric_limits<U>::max());
    }
    point<> min(const T& init) {
        T min = init;
        for_each([&](const auto&, const auto& v) {
            min = std::min(min, v);
        });
        return min;
    }

    template<typename U = T, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    point<> argmin() const {
        return argmin(std::numeric_limits<U>::max());
    }
    point<> argmin(const T& init) {
        aoc::point<> p;
        T min = init;
        for_each([&](const auto& cand, const auto& v) {
            if (v < min) {
                min = min;
                p = cand;
            }
        });
        return p;
    }

private:
    friend std::ostream& operator<<(std::ostream& os, const matrix& m) {
        std::vector<size_t> longest_sizes(m.y());
        matrix<std::string> as_strings(m.x(), m.y());
        for (ptrdiff_t row = 0; row < m.x(); ++row) {
            for (ptrdiff_t col = 0; col < m.y(); ++col) {
                const auto& str = as_strings(row, col) = repr_str(m(row, col));
                longest_sizes[col] = std::max(longest_sizes[col], str.size());
            }
        }
        for (ptrdiff_t col = 0; col < m.y(); ++col) {
            const auto max_size = longest_sizes[col];
            for (ptrdiff_t row = 0; row < m.x(); ++row) {
                auto& str = as_strings(row, col);
                str.insert(0, max_size - str.size(), ' ');
            }
        }

        os << '{';
        ptrdiff_t row = 0;
        ptrdiff_t col = 0;
        for (const auto& val : as_strings.flat()) {
            os << val;
            ++col;
            if (col == m.y()) {
                col = 0;
                ++row;
                if (row == m.x()) {
                    os << '}';
                }
                else {
                    os << "\n ";
                }
            }
            else {
                os << ", ";
            }
        }
        return os;
    }

    std::vector<T> storage_;
    ptrdiff_t x_;
    ptrdiff_t y_;
};
}  // namespace aoc
