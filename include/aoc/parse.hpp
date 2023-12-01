#pragma once

#include <charconv>
#include <concepts>
#include <stdexcept>
#include <string_view>

#include "aoc/demangle.hpp"
#include "aoc/repr.hpp"

namespace aoc {
using namespace std::literals;

template<typename T>
class bad_parse : public std::exception {
public:
    bad_parse(std::string_view data)
        : msg_("failed to parse "s + aoc::type_name<T>() +
               " from: " + aoc::repr_str(data)) {}

    const char* what() const noexcept override { return msg_.data(); }

private:
    std::string msg_;
};

template<>
class bad_parse<void> : public std::exception {
public:
    bad_parse(std::string_view data)
        : msg_("failed to parse input from: " + aoc::repr_str(data)) {}

    const char* what() const noexcept override { return msg_.data(); }

private:
    std::string msg_;
};

namespace detail {
template<std::integral T>
T parse_integral_no_sign(std::string_view sv, int base = 10) {
    T out;
    auto [p, ec] = std::from_chars(sv.begin(), sv.end(), out, base);
    if (ec != std::errc{} || p != sv.end()) {
        throw bad_parse<T>{sv};
    }
    return out;
}
}  // namespace detail

template <typename T>
struct parse_fn;

template <std::integral T>
struct parse_fn<T> {
    constexpr inline auto operator()(std::string_view sv, int base = 10) const -> T {
        if (sv.empty()) {
            throw bad_parse<T>{sv};
        }
        if (sv[0] == '-') {
            sv.remove_prefix(1);
            return -detail::parse_integral_no_sign<T>(sv, base);
        }
        if (sv[0] == '+') {
            sv.remove_prefix(1);
        }
        return detail::parse_integral_no_sign<T>(sv, base);
    }
};

template <std::floating_point T>
struct parse_fn<T> {
    constexpr inline auto operator()(std::string_view sv) const ->  T {
        T out;
        auto [p, ec] = std::from_chars(sv.begin(), sv.end(), out);
        if (ec != std::errc{} || p != sv.end()) {
            throw bad_parse<T>{sv};
        }
        return out;
    }
};

template <typename T>
constexpr inline parse_fn<T> parse{};
}  // namespace aoc
