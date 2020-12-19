#pragma once

#include <string_view>
#include <tuple>

namespace aoc {
namespace detail {
/** Combine two hashes into one. This algorithm is taken from `boost`.
 */
template<typename T>
T bin_hash_combine(T hash_1, T hash_2) {
    return hash_1 ^ (hash_2 + 0x9e3779b9 + (hash_1 << 6) + (hash_1 >> 2));
}
}  // namespace detail

/** Combine two or more hashes into one.
 */
template<typename T, typename... Ts>
auto hash_combine(T head, Ts... tail) {
    ((head = detail::bin_hash_combine(head, tail)), ...);
    return head;
}

/** Hash multiple values by `hash_combine`ing them together.
 */
template<typename... Ts>
auto hash_many(const Ts&... vs) {
    return hash_combine(std::hash<Ts>{}(vs)...);
}

template<typename T>
concept has_std_hash = requires(const T& t) {
    std::hash<T>{}(t);
};

template<has_std_hash T>
auto hash_value(const T& v) {
    return std::hash<T>{}(v);
}

template<typename T>
concept has_hash_member = requires(const T& t) {
    { t.hash() } -> std::convertible_to<std::size_t>;
};

template<has_hash_member T>
auto hash_value(const T& v) {
    return v.hash();
}

/** Hash a tuple by `hash_many`ing all the values together.
 */
template<typename... Ts>
auto hash_value(const std::tuple<Ts...>& t) {
    return std::apply<decltype(hash_many<Ts...>)>(hash_many, t);
}

template<typename T>
concept hashable = requires(const T& t) {
    hash_value(t);
    { t == t } -> std::convertible_to<bool>;
};

struct hasher {
    template<hashable T>
    auto operator()(const T& v) const {
        return hash_value(v);
    }
};
}  // namespace aoc
