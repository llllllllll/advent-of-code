#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <unordered_set>
#include <vector>

template<typename T>
class vec2 {
private:
    T m_x;
    T m_y;

public:
    constexpr T x() const {
        return m_x;
    }

    constexpr T y() const {
        return m_y;
    }

    vec2() = default;
    constexpr vec2(T x, T y) : m_x(x), m_y(y) {}

    constexpr T cross(vec2 other) const {
        return x() * other.y() - y() * other.x();
    }

    constexpr T operator*(vec2 other) const {
        return x() * other.x() + y() * other.y();
    }

    constexpr vec2 operator+(vec2 other) const {
        return {x() + other.x(), y() + other.y()};
    }

    constexpr vec2 operator-(vec2 other) const {
        return {x() - other.x(), y() - other.y()};
    }

    constexpr bool operator==(vec2 other) const {
        return std::abs(x() - other.x()) <= std::numeric_limits<T>::epsilon() &&
               std::abs(y() - other.y()) <= std::numeric_limits<T>::epsilon();
    }

    constexpr bool operator!=(vec2 other) const {
        return !(*this == other);
    }

    constexpr auto size() const {
        return std::sqrt(*this * *this);
    }

    constexpr bool close(vec2 other, double dist) const {
        return std::abs((*this - other).size()) < dist;
    }
};

namespace std {
template<typename T>
struct hash<vec2<T>> {
    std::size_t operator()(vec2<T> v) const noexcept {
        return std::hash<T>{}(v.x()) * std::hash<T>{}(v.y());
    }
};
}  // namespace std

std::tuple<vec2<int>, int> max_visible(const std::unordered_set<vec2<int>>& points) {
    std::size_t max_visible = 0;
    vec2<int> best_spot;

    for (vec2<int> p : points) {
        std::unordered_set<double> conncted_directions;
        for (vec2<int> q : points) {
            if (p == q) {
                continue;
            }
            vec2<int> diff = q - p;
            conncted_directions.emplace(std::atan2(diff.y(), diff.x()));
        }

        if (conncted_directions.size() > max_visible) {
            max_visible = conncted_directions.size();
            best_spot = p;
        }
    }

    return {best_spot, max_visible};
}

vec2<int>
nth_destroyed(int n, vec2<int> center, const std::unordered_set<vec2<int>>& points) {
    constexpr double pi = std::acos(-1);

    struct dist_cmp {
        bool operator()(vec2<int> a, vec2<int> b) const {
            return a.size() < b.size();
        }
    };

    std::map<double, std::set<vec2<int>, dist_cmp>> by_angle;

    for (vec2<int> p : points) {
        if (p != center) {
            vec2<int> diff = p - center;
            by_angle[std::atan2(diff.y(), diff.x())].emplace(diff);
        }
    }

    auto it = by_angle.begin();
    for (; it != by_angle.end(); ++it) {
        if (it->first >= -pi / 2) {
            break;
        }
    }
    for (int ix = 0; ix < n - 1; ++ix) {
        it->second.erase(it->second.begin());
        auto old_it = it;
        ++it;
        if (it == by_angle.end()) {
            it = by_angle.begin();
        }
        if (!old_it->second.size()) {
            by_angle.erase(old_it);
        }
    }
    return *it->second.begin() + center;
}

template<typename T>
std::unordered_set<vec2<T>> parse_map(std::istream& in) {
    std::unordered_set<vec2<T>> out;
    T x = 0;
    T y = 0;

    char c;
    while (in.get(c)) {
        if (c == '#') {
            out.emplace(x, y);
        }
        else if (c == '\n') {
            x = 0;
            ++y;
            continue;
        }
        ++x;
    }

    return out;
}

int main() {
    auto map = parse_map<int>(std::cin);

    std::cout << "\n";
    auto [p, visible] = max_visible(map);
    vec2<int> destroyed = nth_destroyed(200, p, map);
    std::cout << '{' << destroyed.x() << ',' << destroyed.y() << "}\n";
}
