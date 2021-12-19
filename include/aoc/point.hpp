#pragma once

#include <functional>
#include <utility>

namespace aoc {
template <typename T = int>
class point {
public:
    point() = default;
    point(const point&) = default;
    point(point&&) = default;

    point(T x, T y) : x_{std::move(x)}, y_{std::move(y)} {}

    const T& x() const { return x_; }
    const T& y() const { return y_; }

    point operator+(const point& other) {
        return {x() + other.x(), y() + other.y()};
    }
    point operator-(const point& other) {
        return {x() - other.x(), y() - other.y()};
    }

    bool operator==(const point&) const = default;
    auto operator<=>(const point&) const = default;

private:
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
