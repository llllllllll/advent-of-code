#include <optional>
#include <regex>

namespace aoc {
using svmatch = std::match_results<std::string_view::const_iterator>;
using svregex_iterator = std::regex_iterator<std::string_view::const_iterator>;

inline std::optional<svmatch> regex_match(std::string_view pattern,
                                          std::string_view str) {
    std::regex p{pattern.cbegin(), pattern.cend()};
    svmatch m;
    if (!std::regex_match(str.cbegin(), str.cend(), m, p)) {
        return {};
    }
    return m;
}

template<typename F>
void for_each_regex_match(std::string_view pattern, std::string_view str, F&& f) {
    std::regex p{pattern.cbegin(), pattern.cend()};
    const auto end = svregex_iterator{};
    for (auto it = svregex_iterator{str.begin(), str.end(), p}; it != end; ++it) {
        std::invoke(f, *it);
    }
}
}  // namespace aoc
