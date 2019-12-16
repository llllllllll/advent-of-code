#include <array>
#include <iostream>
#include <numeric>
#include <regex>
#include <unordered_set>

using axis = std::array<int, 4>;

struct state {
    axis pos;
    axis vel;

    bool operator==(const state& other) const {
        return pos == other.pos && vel == other.vel;
    }

    bool operator!=(const state& other) const {
        return !(*this == other);
    }
};

namespace std {
template<>
struct hash<state> {
private:
    template<typename T>
    T bin_hash_combine(T hash_1, T hash_2) const {
        return hash_1 ^ (hash_2 + 0x9e3779b9 + (hash_1 << 6) + (hash_1 >> 2));
    }

    /** Combine two or more hashes into one.
     */
    template<typename T, typename... Ts>
    auto hash_combine(T head, Ts... tail) const {
        ((head = bin_hash_combine(head, tail)), ...);
        return head;
    }

public:
    std::size_t operator()(const state& s) const {
        return hash_combine(s.pos[0],
                            s.pos[1],
                            s.pos[2],
                            s.pos[3],
                            s.vel[0],
                            s.vel[1],
                            s.vel[2],
                            s.vel[3]);
    }
};
}  // namespace std

state update(const state& s) {
    state out = s;
    for (std::size_t ix = 0; ix < s.pos.size(); ++ix) {
        for (std::size_t jx = ix + 1; jx < s.pos.size(); ++jx) {
            if (s.pos[ix] > s.pos[jx]) {
                --out.vel[ix];
                ++out.vel[jx];
            }
            else if (s.pos[ix] < s.pos[jx]) {
                ++out.vel[ix];
                --out.vel[jx];
            }
        }
    }

    for (std::size_t ix = 0; ix < s.pos.size(); ++ix) {
        out.pos[ix] += out.vel[ix];
    }

    return out;
}

std::size_t search_space(state s) {
    std::unordered_set<state> states;

    std::size_t step = 0;
    while (states.count(s) == 0) {
        states.emplace(s);
        s = update(s);
        ++step;
    }

    return step;
}

std::array<state, 3> parse(std::istream& in) {
    std::array<state, 3> out = {0};

    std::stringstream ss;
    ss << in.rdbuf();
    std::string data = ss.str();

    std::size_t ix = 0;

    std::regex p{"<x=(-?\\d+), y=(-?\\d+), z=(-?\\d+)>"};
    for (auto it = std::sregex_iterator(data.begin(), data.end(), p);
         it != std::sregex_iterator{};
         ++it) {
        std::smatch m = *it;

        out[0].pos[ix] = std::stoi(m[1]);
        out[1].pos[ix] = std::stoi(m[2]);
        out[2].pos[ix] = std::stoi(m[3]);
        ++ix;
    }

    return out;
}

int main() {
    auto states = parse(std::cin);

    std::size_t x_steps = search_space(states[0]);
    std::size_t y_steps = search_space(states[1]);
    std::size_t z_steps = search_space(states[2]);

    std::cout << "x: " << x_steps << ", y: " << y_steps << ", z: " << z_steps << '\n';
    std::cout << "lcm: " << std::lcm(std::lcm(x_steps, y_steps), z_steps)<< '\n';
}
