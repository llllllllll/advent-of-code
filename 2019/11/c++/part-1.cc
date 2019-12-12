#include <functional>
#include <iostream>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <sys/mman.h>

using word = long long;

enum class opcode : char {
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

enum class access_mode : char {
    pos = 0,
    imm = 1,
    rel = 2,
};

struct instruction {
    opcode op;
    access_mode m1;
    access_mode m2;
    access_mode m3;

private:
    constexpr word ipow(word n, word digit) {
        word out = n;
        for (word ix = 0; ix < digit - 1; ++ix) {
            out *= n;
        }
        return out;
    }

    constexpr word digit(word n, word digit) {
        return (n % ipow(10, digit)) / ipow(10, digit - 1);
    }

public:
    instruction(word encoded)
        : op(static_cast<opcode>(encoded % 100)),
          m1(static_cast<access_mode>(digit(encoded, 3))),
          m2(static_cast<access_mode>(digit(encoded, 4))),
          m3(static_cast<access_mode>(digit(encoded, 5))) {}
};

class memory_space {
    std::size_t m_size;
    word* m_data;

    static word* alloc(std::size_t size) {
        word* out = reinterpret_cast<word*>(
            mmap(0,
                 size,
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                 -1,
                 0));
        if (out == MAP_FAILED) {
            throw std::bad_alloc{};
        }
        return out;
    }

public:
    memory_space() : m_size(0), m_data(nullptr) {}

    memory_space(std::size_t size) : m_size(size), m_data(alloc(size)) {}

    memory_space(const memory_space& cpfrom)
        : m_size(cpfrom.m_size), m_data(alloc(cpfrom.m_size)) {}

    memory_space(memory_space&& mvfrom) : m_size(mvfrom.m_size), m_data(mvfrom.m_data) {
        mvfrom.m_size = 0;
        mvfrom.m_data = nullptr;
    }

    word operator[](std::ptrdiff_t ix) const {
        return m_data[ix];
    }

    word& operator[](std::ptrdiff_t ix) {
        return m_data[ix];
    }

    ~memory_space() {
        if (m_data) {
            munmap(m_data, m_size);
        }
    }
};

class machine {
public:
    enum class state {
        running,
        halt,
        input,
        output,
    };

private:
    std::size_t m_ip;
    word m_offset_reg;
    memory_space m_prog;
    state m_state;

public:
    machine() = default;

    machine(std::string_view prog)
        : m_ip(0), m_offset_reg(0), m_prog(1L << 25), m_state(state::running) {
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

        for (char c : prog) {
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

private:
    word deref(std::size_t ix, bool offset) const {
        word val = m_prog[ix];
        if (offset) {
            val += m_offset_reg;
        }
        return m_prog[val];
    }

    word& deref(std::size_t ix, bool offset) {
        word val = m_prog[ix];
        if (offset) {
            val += m_offset_reg;
        }
        return m_prog[val];
    }

    word raccess(access_mode mode, std::size_t ix) const {
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

    instruction next_instr() {
        return instruction{m_prog[m_ip]};
    }

    void add(instruction instr) {
        waccess(instr.m3, m_ip + 3) = raccess(instr.m1, m_ip + 1) +
                                      raccess(instr.m2, m_ip + 2);
        m_ip += 4;
    }

    void mul(instruction instr) {
        waccess(instr.m3, m_ip + 3) = raccess(instr.m1, m_ip + 1) *
                                      raccess(instr.m2, m_ip + 2);
        m_ip += 4;
    }

    void inp(instruction instr, word input) {
        if (m_state != state::input) {
            m_state = state::input;
            return;
        }
        waccess(instr.m1, m_ip + 1) = input;
        m_ip += 2;
        m_state = state::running;
    }

    word outp(instruction instr) {
        word out = raccess(instr.m1, m_ip + 1);
        m_ip += 2;
        m_state = state::output;
        return out;
    }

    void jit(instruction instr) {
        if (raccess(instr.m1, m_ip + 1)) {
            m_ip = raccess(instr.m2, m_ip + 2);
        }
        else {
            m_ip += 3;
        }
    }

    void jif(instruction instr) {
        if (!raccess(instr.m1, m_ip + 1)) {
            m_ip = raccess(instr.m2, m_ip + 2);
        }
        else {
            m_ip += 3;
        }
    }

    void lt(instruction instr) {
        waccess(instr.m3, m_ip + 3) = raccess(instr.m1, m_ip + 1) <
                                      raccess(instr.m2, m_ip + 2);
        m_ip += 4;

        instruction next = next_instr();
        switch (next.op) {
        case opcode::jit:
            return jit(next);
        case opcode::jif:
            return jif(next);
        default:;
        }
    }

    void eq(instruction instr) {
        waccess(instr.m3, m_ip + 3) = raccess(instr.m1, m_ip + 1) ==
                                      raccess(instr.m2, m_ip + 2);
        m_ip += 4;

        instruction next = next_instr();
        switch (next.op) {
        case opcode::jit:
            return jit(next);
        case opcode::jif:
            return jif(next);
        default:;
        }
    }

    void rbo(instruction instr) {
        m_offset_reg += raccess(instr.m1, m_ip + 1);
        m_ip += 2;
    }


    void halt() {
        m_state = state::halt;
    }

public:
    struct suspension {
        state st;
        word val;

        explicit operator bool() const {
            return st != state::halt;
        }
    };

    suspension step(word input = 0) {
        if (m_state != state::input && input != 0) {
            throw std::runtime_error{"cannot give input when not waiting for input"};
        }
        if (m_state == state::output) {
            m_state = state::running;
        }

        word out = 0;

        do {
            instruction instr = next_instr();
            switch (instr.op) {
            case opcode::add:
                add(instr);
                break;
            case opcode::mul:
                mul(instr);
                break;
            case opcode::inp:
                inp(instr, input);
                break;
            case opcode::outp:
                out = outp(instr);
                break;
            case opcode::jit:
                jit(instr);
                break;
            case opcode::jif:
                jif(instr);
                break;
            case opcode::lt:
                lt(instr);
                break;
            case opcode::eq:
                eq(instr);
                break;
            case opcode::rbo:
                rbo(instr);
                break;
            case opcode::halt:
                halt();
                break;
            default:
                __builtin_unreachable();
            }
        } while (m_state == state::running);

        return {m_state, out};
    }
};

enum class color : word {
    black = 0,
    white = 1,
};


class robot {
    enum class direction : char {
        up,
        down,
        left,
        right,
    };

    machine m_machine;
    direction m_direction;
    std::tuple<int, int> m_coord;

    void turn_left() {
        switch(m_direction) {
        case direction::up:
            m_direction = direction::left;
            return;
        case direction::down:
            m_direction = direction::right;
            return;
        case direction::left:
            m_direction = direction::down;
            return;
        case direction::right:
            m_direction = direction::up;
            return;
        default:
            __builtin_unreachable();
        }
    }

    void turn_right() {
        switch(m_direction) {
        case direction::up:
            m_direction = direction::right;
            return;
        case direction::down:
            m_direction = direction::left;
            return;
        case direction::left:
            m_direction = direction::up;
            return;
        case direction::right:
            m_direction = direction::down;
            return;
        default:
            __builtin_unreachable();
        }
    }

    void step_forward() {
        auto [x, y] = m_coord;
        switch (m_direction) {
        case direction::up:
            m_coord = {x, y + 1};
            return;
        case direction::down:
            m_coord = {x, y - 1};
            return;
        case direction::left:
            m_coord = {x - 1, y};
            return;
        case direction::right:
            m_coord = {x + 1, y};
            return;
        default:
            __builtin_unreachable();
        }
    }


public:
    robot(std::string_view prog)
        : m_machine(prog), m_direction(direction::up), m_coord(0, 0) {
        auto [st, v] = m_machine.step();
        if (st != machine::state::input) {
            throw std::runtime_error{"failed to prime machine"};
        }
    }

    std::tuple<std::tuple<int, int>, color, bool> step(color c) {
        bool more = true;
        color new_color;
        auto s = m_machine.step(static_cast<word>(c));
        if (s.st != machine::state::output) {
            throw std::runtime_error{"machine didn't output"};
        }
        new_color = color{s.val};

        s = m_machine.step();
        if (s.st != machine::state::output) {
            throw std::runtime_error{"machine didn't output"};
        }
        if (s.val) {
            turn_left();
        }
        else {
            turn_right();
        }
        step_forward();


        s = m_machine.step();
        if (s.st == machine::state::halt) {
            more = false;
        }
        else if (s.st != machine::state::input) {
            throw std::runtime_error{"machine isn't waiting on input"};
        }

        return {m_coord, new_color, more};
    }
};

namespace std {
template<>
struct hash<std::tuple<int, int>> {
    std::size_t operator()(std::tuple<int, int> v) const noexcept {
        auto [a, b] = v;
        return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
    }
};
}  // namespace std

int check_num_painted(std::string_view prog) {
    robot r(prog);

    std::unordered_map<std::tuple<int, int>, color> tile_color;
    std::unordered_set<std::tuple<int, int>> painted;

    std::tuple<int, int> coord = {0, 0};
    while (true) {
        auto [p, color, more] = r.step(tile_color[coord]);
        painted.emplace(coord);
        tile_color[coord] = color;
        coord = p;
        if (!more) {
            break;
        }
    }

    return painted.size();
}

int main() {
    std::string prog;
    std::cin >> prog;

    std::cout << check_num_painted(prog) << '\n';
}
