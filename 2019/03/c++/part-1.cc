#include <iostream>
#include <limits>
#include <optional>
#include <vector>

template<typename T>
class vec2 {
private:
    T m_x;
    T m_y;

public:
    T x() const {
        return m_x;
    }

    T y() const {
        return m_y;
    }

public:
    vec2() = default;
    vec2(T x, T y) : m_x(x), m_y(y) {}

    T cross(vec2 other) const {
        return x() * other.y() - y() * other.x();
    }

    T operator*(vec2 other) const {
        return x() * other.x() + y() * other.y();
    }

    vec2 operator+(vec2 other) const {
        return {x() + other.x(), y() + other.y()};
    }

    vec2 operator-(vec2 other) const {
        return {x() - other.x(), y() - other.y()};
    }

    T mdist() const {
        return std::abs(x()) + std::abs(y());
    }
};

template<typename T>
class segment {
private:
    vec2<T> m_start;
    vec2<T> m_end;

public:
    const vec2<T>& start() const {
        return m_start;
    }

    const vec2<T>& end() const {
        return m_end;
    }

    segment(vec2<T> start, vec2<T> end) : m_start(start), m_end(end) {}

    std::optional<vec2<T>> operator&(segment other) const {
        auto s1 = end() - start();
        auto s2 = other.end() - other.start();
        auto start_diff = start() - other.start();

        T s_d = -s2.x() * s1.y() + s1.x() * s2.y();
        if (s_d == 0) {
            return {};
        }
        T s = s1.cross(start_diff) / s_d;

        T t_d = -s2.x() * s1.y() + s1.x() * s2.y();
        if (t_d == 0) {
            return {};
        }
        T t = s2.cross(start_diff) / t_d;

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
            return vec2<T>{start().x() + (t * s1.x()), start().y() + (t * s1.y())};
        }
        return {};
    }
};

template<typename T>
std::vector<segment<T>> parse_segments(std::istream& stream) {
    vec2<float> current_loc{0, 0};

    std::vector<segment<T>> out;

    while (true) {
        char direction;
        if (!stream.get(direction) || direction == '\n') {
            break;
        }

        int dist = 0;
        char c;
        while (stream.get(c) && (c >= '0' && c <= '9')) {
            dist *= 10;
            dist += c - '0';
        }

        vec2<T> end;
        switch (direction) {
        case 'L':
            end = vec2{current_loc.x() - dist, current_loc.y()};
            break;
        case 'R':
            end = vec2{current_loc.x() + dist, current_loc.y()};
            break;
        case 'U':
            end = vec2{current_loc.x(), current_loc.y() + dist};
            break;
        case 'D':
            end = vec2{current_loc.x(), current_loc.y() - dist};
            break;
        }
        out.emplace_back(current_loc, end);

        if (c == '\n') {
            break;
        }
        current_loc = end;
    }

    return out;
}

int main() {
    auto line_1 = parse_segments<float>(std::cin);
    auto line_2 = parse_segments<float>(std::cin);


    int smallest_mdist = std::numeric_limits<int>::max();

    for (const auto& seg_1 : line_1) {
        for (const auto& seg_2 : line_2) {
            auto cross = seg_1 & seg_2;
            if (cross && cross->mdist() > 0 && cross->mdist() < smallest_mdist) {
                smallest_mdist = cross->mdist();
            }
        }
    }
    std::cout << smallest_mdist << '\n';
}
