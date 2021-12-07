#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <functional>

namespace aoc {
constexpr inline std::string_view whitespace{" \r\n\t"};

template <typename F>
void for_split_first_of(std::string_view sv, std::string_view delim, F&& f) {
    std::size_t end;
    while ((end = sv.find_first_of(delim)) != std::string_view::npos) {
        std::invoke(f, sv.substr(0, end));
        sv.remove_prefix(end + 1);
    }
    std::invoke(f, sv.substr(0, end));
}

template <typename F>
void for_split(std::string_view sv, std::string_view delim, F&& f) {
    std::size_t end;
    while ((end = sv.find(delim)) != std::string_view::npos) {
        std::invoke(f, sv.substr(0, end));
        sv.remove_prefix(end + delim.size());
    }
    std::invoke(f, sv.substr(0, end));
}

template <typename F>
void for_split(std::string_view sv, char delim, F&& f) {
    return for_split(sv, std::string_view{&delim, 1}, std::forward<F>(f));
}

inline std::vector<std::string> split(std::string_view sv, std::string_view delim) {
    std::vector<std::string> out;
    for_split(sv, delim, [&](auto v) { out.emplace_back(v); });
    return out;
}

inline std::vector<std::string> split(std::string_view sv, char delim) {
    return split(sv, std::string_view{&delim, 1});
}

inline bool starts_with(std::string_view cs, std::string_view prefix) {
    return cs.rfind(prefix, 0) == 0;
}

inline std::string_view strip_view(std::string_view cs) {
    const auto begin = cs.find_first_not_of(whitespace);
    const auto end = cs.find_last_not_of(whitespace);
    return cs.substr(begin, end - begin + 1);
}

inline std::string strip(std::string_view sv) {
    return std::string{strip_view(sv)};
}
}  // namespace aoc
