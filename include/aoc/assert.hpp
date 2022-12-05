#pragma once

#include <iostream>

namespace aoc::detail {
[[noreturn]] inline void assert_failed(std::string_view file,
                                       size_t line,
                                       std::string_view expr) {
    std::cerr << file << ':' << line << ": " << "Assertion failed: " << expr << '\n';
    std::terminate();
}
}  // namespace aoc::detail

#define AOC_ASSERT(expr) \
    (__builtin_expect(!!(expr), 1) \
     ? static_cast<void>(0) \
     : ::aoc::detail::assert_failed(__FILE__, __LINE__, #expr))
