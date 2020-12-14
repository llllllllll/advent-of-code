#include <stdexcept>

namespace aoc {
struct no_solution_error : public std::exception {
    const char* what() const noexcept override {
        return "no solution";
    }
};

[[noreturn]] void no_solution() {
    throw no_solution_error{};
}
}  // namespace aoc
