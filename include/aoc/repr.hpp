#pragma once

#include <iostream>
#include <ranges>
#include <type_traits>

namespace aoc {
namespace detail {
template<typename T, typename>
struct repr;

template<typename T>
struct repr<T, void> {
    const T& value;

    friend std::ostream& operator<<(std::ostream& os, repr r) {
        return os << r.value;
    }
};

template<typename T>
requires std::is_convertible_v<T, std::string_view>
struct repr<T, void> {
    std::string_view value;

    friend std::ostream& operator<<(std::ostream& os, repr r) {
        os << '"';
        for (char c : r.value) {
            switch (c) {
            case '"':
                os << '\\' << '"';
                break;
            case '\\':
                os << '\\' << '\\';
                break;
            case '\0':
                os << '\\' << '0';
                break;
            case '\r':
                os << '\\' << 'r';
                break;
            case '\n':
                os << '\\' << 'n';
                break;
            case '\t':
                os << '\\' << 't';
                break;
            default:
                os << c;
            }
        }
        os << '"';
        return os;
    }
};

template<std::ranges::range T>
struct repr<T, void> {
    const T& value;

    friend std::ostream& operator<<(std::ostream& os, repr r) {
        os << "{";
        auto begin = std::ranges::begin(r.value);
        const auto end = std::ranges::end(r.value);
        if (begin != end) {
            os << repr(*begin);
            ++begin;
        }
        for (auto it = begin; it != end; ++it) {
            os << ", " << repr(*it);
        }
        os << "}";
        return os;
    }
};
}  // namespace dispatch

template<typename T>
auto repr(const T& r) {
    return detail::repr{r};
}
}  // namespace aoc
