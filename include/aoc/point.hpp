#pragma once

#include <functional>
#include <ostream>
#include <utility>

namespace aoc {
template <typename T = ptrdiff_t>
class point {
public:
    point() = default;
    point(const point&) = default;
    point(point&&) = default;

    point(T x, T y) : x_{std::move(x)}, y_{std::move(y)} {}

    const T& x() const { return x_; }
    const T& y() const { return y_; }

    point operator+(const point& other) const {
        return {x() + other.x(), y() + other.y()};
    }
    point operator-(const point& other) const {
        return {x() - other.x(), y() - other.y()};
    }

    bool operator==(const point&) const = default;
    auto operator<=>(const point&) const = default;

private:
    friend std::ostream& operator<<(std::ostream& os, const point& p) {
        return os << '(' << p.x() << ", " << p.y() << ')';
    }

    T x_;
    T y_;
};
}  // namespace aoc

namespace std {
template <typename T>
struct hash<aoc::point<T>> {
    size_t operator()(const aoc::point<T>& p) const noexcept {
        hash<T> h{};
        return h(p.x()) * h(p.y());
    }
};
}  // namespace std
