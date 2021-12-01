#pragma once

#include <iostream>
#include <ranges>
#include <sstream>
#include <type_traits>

#include "aoc/demangle.hpp"

namespace aoc {
template<typename F>
struct printer {
    F f;

    friend std::ostream& operator<<(std::ostream& os, const printer& p) {
        return p.f(os);
    }
};


namespace detail {
template<typename T>
concept string_like = std::is_convertible_v<T, std::string_view>;

template<typename T>
concept non_string_range = !string_like<T> && std::ranges::range<T>;

template<typename T>
concept tuple_like = requires {
    std::tuple_size<T>{};
};

template<typename T>
concept enum_like = std::is_enum_v<T>;
}  // namespace detail


auto repr(char c) {
    return printer([c](std::ostream& os) -> std::ostream& {
        return os << '\'' << c << '\'';
    });
}

template<typename T>
auto repr(const T& val) {
    return printer{[&](std::ostream& os) -> std::ostream& { return os << val; }};
}

template<detail::enum_like E>
auto repr(const E& val) {
    return printer([&](std::ostream& os) -> std::ostream& {
        return os << aoc::type_name<E>() << '{'
                  << repr(static_cast<std::underlying_type_t<E>>(val)) << '}';
    });
}

template<detail::string_like T>
auto repr(const T& val) {
    return printer{[&](std::ostream& os) -> std::ostream& {
        os << '"';
        for (char c : std::string_view{val}) {
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
    }};
}


namespace detail {
template<typename T, std::size_t... Ixs>
std::ostream& print_tuple(std::ostream& os, const T& tup, std::index_sequence<Ixs...>) {
    os << "(";
    (..., (os << (Ixs == 0 ? "" : ", ") << repr(std::get<Ixs>(tup))));
    os << ")";
    return os;
}
}  // namespace detail

template<detail::tuple_like T>
auto repr(const T& val) {
    return printer{[&](std::ostream& os) -> std::ostream& {
        return detail::print_tuple(os,
                                   val,
                                   std::make_index_sequence<std::tuple_size_v<T>>{});
    }};
}

template<detail::non_string_range T>
auto repr(const T& val) {
    return printer{[&](std::ostream& os) -> std::ostream& {
        os << "{";
        auto begin = std::ranges::begin(val);
        const auto end = std::ranges::end(val);
        if (begin != end) {
            os << repr(*begin);
            ++begin;
        }
        for (auto it = begin; it != end; ++it) {
            os << ", " << repr(*it);
        }
        os << "}";
        return os;
    }};
}

template<typename T>
std::string repr_str(const T& val) {
    std::stringstream ss;
    ss << aoc::repr(val);
    return ss.str();
}

template<typename T, typename... Ts>
auto dbg(const T& head, const Ts&... tail) {
    std::cerr << repr(head);
    if constexpr (sizeof...(tail)) {
        std::cerr << ' ';
        dbg(tail...);
    }
    else {
        std::cerr << std::endl;
    }
}

}  // namespace aoc
