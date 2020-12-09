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

    const char* what() const noexcept override {
        return msg_.data();
    }

private:
    std::string msg_;
};

namespace detail {
template<std::integral T>
T parse_integral_no_sign(std::string_view sv) {
    T out;
    auto [p, ec] = std::from_chars(sv.begin(), sv.end(), out);
    if (ec != std::errc{} || p != sv.end()) {
        throw bad_parse<T>{sv};
    }
    return out;
}
}  // namespace detail

template<std::integral T>
T parse(std::string_view sv) {
    if (sv.empty()) {
        throw bad_parse<T>{sv};
    }
    if (sv[0] == '-') {
        sv.remove_prefix(1);
        return -detail::parse_integral_no_sign<T>(sv);
    }
    if (sv[0] == '+') {
        sv.remove_prefix(1);
    }
    return detail::parse_integral_no_sign<T>(sv);
}
}  // namespace aoc
