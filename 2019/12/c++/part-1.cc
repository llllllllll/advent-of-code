#include <array>
#include <cmath>
#include <iostream>
#include <regex>
#include <vector>

template<typename T, std::size_t dim>
class vecn {
private:
    std::array<T, dim> m_data;

public:
    std::array<T, dim>& arr() {
        return m_data;
    }

    const std::array<T, dim>& arr() const {
        return m_data;
    }

    vecn() : m_data({0}) {}

    template<typename... Ts>
    constexpr vecn(Ts... data) : m_data({data...}) {}

    constexpr explicit vecn(std::array<T, dim> data) : m_data(data) {}

    constexpr T operator*(vecn other) const {
        T out = 0;
        for (std::size_t ix = 0; ix < dim; ++ix) {
            out += m_data[ix] * other.m_data[ix];
        }
        return out;
    }

    constexpr vecn operator+(vecn other) const {
        std::array<T, dim> new_data;
        for (std::size_t ix = 0; ix < dim; ++ix) {
            new_data[ix] = m_data[ix] + other.m_data[ix];
        }
        return vecn{new_data};
    }

    const vecn& operator+=(vecn other) {
        for (std::size_t ix = 0; ix < dim; ++ix) {
            m_data[ix] += other.m_data[ix];
        }
        return *this;
    }

    constexpr vecn operator-(vecn other) const {
        std::array<T, dim> new_data;
        for (std::size_t ix = 0; ix < dim; ++ix) {
            new_data[ix] = m_data[ix] - other.m_data[ix];
        }
        return vecn{new_data};
    }

    const vecn& operator-=(vecn other) {
        for (std::size_t ix = 0; ix < dim; ++ix) {
            m_data[ix] -= other.m_data[ix];
        }
        return *this;
    }

    constexpr bool operator==(vecn other) const {
        for (std::size_t ix = 0; ix < dim; ++ix) {
            if (m_data[ix] != other.m_data[ix]) {
                return false;
            }
        }
        return true;
    }

    constexpr bool operator!=(vecn other) const {
        return !(*this == other);
    }

    constexpr auto size() const {
        return std::sqrt(*this * *this);
    }

    constexpr T sumabs() const {
        T out = 0;
        for (std::size_t ix = 0; ix < dim; ++ix) {
            out += std::abs(m_data[ix]);
        }
        return out;
    }
};

template<typename T>
using vec3 = vecn<T, 3>;

template<typename T>
std::ostream& operator<<(std::ostream& s, vec3<T> v) {
    auto format_int = [](auto v) {
        if (v >= 0) {
            return " " + std::to_string(v);
        }
        else {
            return std::to_string(v);
        }
    };
    return s << "<x=" << format_int(v.arr()[0]) << ", y=" << format_int(v.arr()[1])
             << ", z=" << format_int(v.arr()[2]) << '>';
}

struct planet {
    vec3<int> pos;
    vec3<int> vel;
};

std::ostream& operator<<(std::ostream& s, const planet& p) {
    return s << "pos=" << p.pos << ", vel=" << p.vel;
}

void update_vel(planet& p, planet& q) {
    for (std::size_t axis : {0, 1, 2}) {
        if (p.pos.arr()[axis] > q.pos.arr()[axis]) {
            --p.vel.arr()[axis];
            ++q.vel.arr()[axis];
        }
        else if (p.pos.arr()[axis] < q.pos.arr()[axis]) {
            ++p.vel.arr()[axis];
            --q.vel.arr()[axis];
        }
    }
}

void update(std::vector<planet>& ps) {
    for (std::size_t ix = 0; ix < ps.size(); ++ix) {
        for (std::size_t jx = ix + 1; jx < ps.size(); ++jx) {
            update_vel(ps[ix], ps[jx]);
        }
    }

    for (planet& p : ps) {
        p.pos += p.vel;
    }
}

std::vector<planet> parse(std::istream& in) {
    std::vector<planet> out;

    std::stringstream ss;
    ss << in.rdbuf();
    std::string data = ss.str();

    std::regex p{"<x=(-?\\d+), y=(-?\\d+), z=(-?\\d+)>"};
    for (auto it = std::sregex_iterator(data.begin(), data.end(), p);
         it != std::sregex_iterator{};
         ++it) {
        std::smatch m = *it;

        planet& p = out.emplace_back();
        p.pos = {std::stoi(m[1]), std::stoi(m[2]), std::stoi(m[3])};
    }

    return out;
}

int main() {
    auto planets = parse(std::cin);

    std::size_t step = 0;

    auto print_planets = [&] {
        std::cout << "After " << step << " steps:\n";
        for (const planet& p : planets) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    };

    print_planets();
    for (step = 1; step <= 1000; ++step) {
        update(planets);
        print_planets();

    }

    int energy = 0;
    for (const planet& p : planets) {
        std::cout << "pot: " << p.pos.sumabs() << ", kin: " << p.vel.sumabs() << '\n';
        energy += p.pos.sumabs() * p.vel.sumabs();
    }

    std::cout << "total energy: " << energy << '\n';
}
