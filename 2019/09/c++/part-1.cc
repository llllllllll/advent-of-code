#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_set>
#include <vector>

using word = long long;

enum class opcode : word {
    add = 1,
    mul = 2,
    inp = 3,
    outp = 4,
    jit = 5,
    jif = 6,
    lt = 7,
    eq = 8,
    rbo = 9,
    halt = 99,
};

enum class access_mode : word {
    pos = 0,
    imm = 1,
    rel = 2,
};

namespace detail {
constexpr std::size_t pow(std::size_t a, std::size_t b) {
    std::size_t out = 1;
    for (std::size_t ix = 0; ix < b; ++ix) {
        out *= a;
    }
    return out;
}
}  // namespace detail

/** A sparse k-ary tree.

    @tparam T The type data stored in the tree.
    @tparam width The width of each node in the tree (k).
    @tparam height The height of the tree.
 */
template<typename T, std::size_t width, std::size_t height>
class ktree {
    using child_type = ktree<T, width, height - 1>;

    std::array<std::unique_ptr<child_type>, width> m_children;

public:
    ktree() = default;
    ktree(const ktree& cpfrom) {
        std::size_t ix = 0;
        for (const auto& child : cpfrom.m_children) {
            if (child) {
                m_children[ix] = std::make_unique<child_type>(*child);
            }
            ++ix;
        }
    }
    ktree(ktree&&) = default;

    constexpr static std::size_t size() {
        return detail::pow(width, height);
    }

    constexpr static std::ptrdiff_t ssize() {
        return detail::pow(width, height);
    }

    constexpr const std::array<std::unique_ptr<child_type>, width>& children() const {
        return m_children;
    }

    constexpr T& operator[](std::ptrdiff_t ix) {
        constexpr std::ptrdiff_t level_size = child_type::ssize();

        std::ptrdiff_t child_ix = ix / level_size;
        std::ptrdiff_t recurse_ix = ix % level_size;
        if (!m_children[child_ix]) {
            m_children[child_ix] = std::make_unique<child_type>();
        }
        return (*m_children[child_ix])[recurse_ix];
    }

    constexpr const T* get(std::ptrdiff_t ix) const {
        constexpr std::ptrdiff_t level_size = child_type::ssize();

        std::ptrdiff_t child_ix = ix / level_size;
        std::ptrdiff_t recurse_ix = ix % level_size;
        if (!m_children[child_ix]) {
            return nullptr;
        }
        return m_children[child_ix]->get(recurse_ix);
    }
};

// recursive base case for height = 1, basically just an array.
template<typename T, std::size_t width>
class ktree<T, width, 1> {
    std::array<T, width> m_children = {0};

public:
    constexpr static std::size_t size() {
        return width;
    }

    constexpr static std::ptrdiff_t ssize() {
        return width;
    }

    constexpr const std::array<T, width>& children() const {
        return m_children;
    }

    constexpr T& operator[](std::ptrdiff_t ix) {
        return m_children[ix];
    }

    constexpr const T* get(std::ptrdiff_t ix) const {
        return &m_children[ix];
    }
};

template<typename T, std::size_t width>
class ktree<T, width, 0> {
    static_assert(!std::is_same_v<T, T>, "cannot have height 0 ktree");
};

struct instruction {
    opcode op;
    access_mode m1;
    access_mode m2;
    access_mode m3;

private:
    word digit(word n, word digit) {
        return (n % static_cast<word>(std::pow(10, digit))) / std::pow(10, digit - 1);
    }

public:
    instruction(word encoded)
        : op(opcode{encoded % 100}),
          m1(access_mode{digit(encoded, 3)}),
          m2(access_mode{digit(encoded, 4)}),
          m3(access_mode{digit(encoded, 5)}) {}
};

class machine {
private:
    std::size_t m_ip;
    word m_offset_reg;
    ktree<word, 8, 22> m_prog;
    std::function<word()> m_input;
    std::function<void(word)> m_output;

public:
    machine() = default;
    machine(const machine&) = default;

    machine(
        std::istream& prog,
        std::function<word()> input =
            [] {
                word out;
                std::cin >> out;
                return out;
            },
        std::function<void(word)> output = [](word n) { std::cout << n << '\n'; })
        : m_ip(0), m_offset_reg(0), m_input(input), m_output(output) {
        word val = 0;
        bool negate = false;
        std::size_t ix = 0;

        auto commit = [&] {
            if (negate) {
                val = -val;
            }
            m_prog[ix++] = val;
            val = 0;
            negate = false;
        };

        char c;
        while (prog.get(c)) {
            if (c == ',') {
                commit();
            }
            else if (c == '-') {
                negate = true;
            }
            else if (c >= '0' && c <= '9') {
                val *= 10;
                val += c - '0';
            }
        }

        commit();
    }

    machine(const machine& m,
            std::function<word()> input,
            std::function<void(word)> output)
        : m_ip(0), m_prog(m.m_prog), m_input(input), m_output(output) {}

private:
    word& deref(std::size_t ix, bool offset) {
        word val = m_prog[ix];
        if (offset) {
            val += m_offset_reg;
        }
        return m_prog[val];
    }

    word raccess(access_mode mode, std::size_t ix) {
        switch (mode) {
        case access_mode::pos:
            return deref(ix, false);
        case access_mode::imm:
            return m_prog[ix];
        case access_mode::rel:
            return deref(ix, true);
        default:
            __builtin_unreachable();
        }
    }

    word& waccess(access_mode mode, std::size_t ix) {
        switch (mode) {
        case access_mode::pos:
            return deref(ix, false);
        case access_mode::imm:
            throw std::runtime_error{"cannot write to immediate"};
        case access_mode::rel:
            return deref(ix, true);
        default:
            __builtin_unreachable();
        }
    }


    class halt {};

    void step() {
        instruction instr{m_prog[m_ip]};
        switch (instr.op) {
        case opcode::add:
            waccess(instr.m3, m_ip + 3) = raccess(instr.m1, m_ip + 1) +
                                          raccess(instr.m2, m_ip + 2);
            m_ip += 4;
            break;
        case opcode::mul:
            waccess(instr.m3, m_ip + 3) = raccess(instr.m1, m_ip + 1) *
                                          raccess(instr.m2, m_ip + 2);
            m_ip += 4;
            break;
        case opcode::inp:
            waccess(instr.m1, m_ip + 1) = m_input();
            m_ip += 2;
            break;
        case opcode::outp:
            m_output(raccess(instr.m1, m_ip + 1));
            m_ip += 2;
            break;
        case opcode::jit:
            if (raccess(instr.m1, m_ip + 1)) {
                m_ip = raccess(instr.m2, m_ip + 2);
            }
            else {
                m_ip += 3;
            }
            break;
        case opcode::jif:
            if (!raccess(instr.m1, m_ip + 1)) {
                m_ip = raccess(instr.m2, m_ip + 2);
            }
            else {
                m_ip += 3;
            }
            break;
        case opcode::lt:
            waccess(instr.m3, m_ip + 3) = raccess(instr.m1, m_ip + 1) <
                                          raccess(instr.m2, m_ip + 2);
            m_ip += 4;
            break;
        case opcode::eq:
            waccess(instr.m3, m_ip + 3) = raccess(instr.m1, m_ip + 1) ==
                                          raccess(instr.m2, m_ip + 2);
            m_ip += 4;
            break;
        case opcode::rbo:
            m_offset_reg += raccess(instr.m1, m_ip + 1);
            m_ip += 2;
            break;
        case opcode::halt:
            throw halt{};
        default:
            __builtin_unreachable();
        }
    }

public:
    void run() {
        try {
            while (true) {
                step();
            }
        }
        catch (halt) {
        }
    }
};

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: part1 PROGRAM\n";
        return 1;
    }
    std::ifstream program(argv[1]);
    if (!program) {
        std::cerr << "failed to open program: " << argv[1] << '\n';
        return 2;
    }
    machine m(program, [] { return 1; });
    m.run();
}
