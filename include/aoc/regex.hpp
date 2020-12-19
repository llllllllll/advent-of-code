#include <optional>
#include <regex>

namespace aoc {
using svmatch = std::match_results<std::string_view::const_iterator>;

std::optional<svmatch> regex_match(std::string_view pattern, std::string_view str) {
    std::regex p{pattern.cbegin(), pattern.cend()};
    svmatch m;
    if (!std::regex_match(str.cbegin(), str.cend(), m, p)) {
        return {};
    }
    return m;
}
}  // namespace aoc
