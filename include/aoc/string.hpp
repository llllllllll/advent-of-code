#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace aoc {
inline std::vector<std::string> split(std::string_view sv, std::string_view delim) {
    std::vector<std::string> out;
    std::size_t end;
    while ((end = sv.find(delim)) != std::string_view::npos) {
        out.emplace_back(sv.substr(0, end));
        sv.remove_prefix(end + delim.size());
    }
    out.emplace_back(sv);
    return out;
}

inline std::vector<std::string> split(std::string_view sv, char delim) {
    return split(sv, std::string_view{&delim, 1});
}

inline std::string strip(std::string_view sv) {
    constexpr std::string_view whitespace = " \r\n\t";
    sv.remove_prefix(sv.find_first_not_of(whitespace));
    sv.remove_suffix(sv.find_last_not_of(whitespace));
    return std::string{sv};
}
}  // namespace aoc
