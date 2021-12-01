#pragma once

#include <sstream>

#include "aoc/repr.hpp"

namespace aoc {
struct no_solution_error : public std::exception {
    const char* what() const noexcept override {
        return "no solution";
    }
};

[[noreturn]] void no_solution() {
    throw no_solution_error{};
}

template<typename T, typename... Args>
T formatted_exc(Args&&... args) {
    std::stringstream s;
    (s << ... << std::forward<Args>(args));
    return T{s.str()};
}
}  // namespace aoc
