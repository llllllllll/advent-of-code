#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <ncurses.h>
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

#if CHEATING
private:
    word m_trash;
#endif

public:
    word operator[](std::ptrdiff_t ix) const {
#if CHEATING
        if (ix == 392){
            ix = 388;
        }
#endif
        return m_data[ix];
    }

    word& operator[](std::ptrdiff_t ix) {
#if CHEATING
        if (ix == 392){
            m_trash = m_data[388];
            return m_trash;
        }
#endif
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

    void set_addr(std::size_t addr, word value) {
        m_prog[addr] = value;
    }

    memory_space& mem() {
        return m_prog;
    }
};

enum class tile_id : char {
    empty = 0,
    wall = 1,
    block = 2,
    paddle = 3,
    ball = 4,
};

class manual_game {
public:
    static std::unordered_set<std::ptrdiff_t> paddle_addresses;
    static std::unordered_set<std::ptrdiff_t> ball_x_addresses;

    machine m_machine;

    struct lost {};

    void read_outputs(word input = 0) {
        while (true) {
            machine::suspension x = m_machine.step(input);
            input = 0;
            if (x.st != machine::state::output) {
                if (x.st == machine::state::halt) {
                    throw lost{};
                }
                return;
            }
            machine::suspension y = m_machine.step();
            if (y.st != machine::state::output) {
                throw std::runtime_error{"unexpected state"};
            }
            machine::suspension id = m_machine.step();
            if (id.st != machine::state::output) {
                throw std::runtime_error{"unexpected state"};
            }
            if (x.val == -1 && y.val == 0) {
                mvprintw(0, 40, "score: %lld", id.val);
            }
            else {
                const char* cs;
                switch (static_cast<tile_id>(id.val)) {
                case tile_id::empty:
                    cs = " ";
                    break;
                case tile_id::wall:
                case tile_id::block:
                    cs = "X";
                    break;
                case tile_id::paddle: {
                    cs = "-";
                    mvprintw(2, 40, "paddle: %lld", x.val);
                    std::unordered_set<std::ptrdiff_t> to_remove;
                    for (std::ptrdiff_t check : paddle_addresses) {
                        if (m_machine.mem()[check] != x.val) {
                            to_remove.emplace(check);
                        }
                    }
                    for (std::size_t r : to_remove) {
                        paddle_addresses.erase(r);
                    }
                    if (paddle_addresses.size() == 1) {
                        mvprintw(3, 40, "paddle addr: %ld", *paddle_addresses.begin());
                    }
                    if (paddle_addresses.size() == 0) {
                        mvprintw(3, 40, "paddle addr: lost");
                    }
                    break;
                }
                case tile_id::ball: {
                    cs = "o";
                    mvprintw(4, 40, "ball %lld,%lld", x.val, y.val);
                    std::unordered_set<std::ptrdiff_t> to_remove_x;
                    for (std::ptrdiff_t check : ball_x_addresses) {
                        if (m_machine.mem()[check] != x.val) {
                            to_remove_x.emplace(check);
                        }
                    }
                    for (std::size_t r : to_remove_x) {
                        ball_x_addresses.erase(r);
                    }
                    if (ball_x_addresses.size() == 1) {
                        mvprintw(5, 40, "ball x addr: %ld", *ball_x_addresses.begin());
                    }
                    if (paddle_addresses.size() == 0) {
                        mvprintw(5, 40, "ball x addr: lost");
                    }
                }
                }
                mvprintw(y.val, x.val, "%s", cs);
            }
        }
    }

public:
    manual_game(std::string_view prog) : m_machine(prog) {
        m_machine.set_addr(0, 2);

        initscr();
        cbreak();
        keypad(stdscr, true);
        curs_set(0);
        noecho();
        mvprintw(0, 40, "00000");
        read_outputs();
    }

private:
    void step() {
        int input = 0;
        switch (getch()) {
        case 'a':
            input = -1;
            break;
        case 'd':
            input = 1;
            break;
        }
        read_outputs(input);
    }

public:
    void play() {
        try {
            while (true) {
                step();
            }
        }
        catch (lost) {
            return;
        }
    }

    ~manual_game() {
        endwin();
    }
};

class cheat_game {
    machine m_machine;
    word m_score = 0;

    struct won {};

    void read_outputs(word input = 0) {
        while (true) {
            machine::suspension x = m_machine.step(input);
            input = 0;
            if (x.st != machine::state::output) {
                if (x.st == machine::state::halt) {
                    throw won{};
                }
                return;
            }
            machine::suspension y = m_machine.step();
            if (y.st != machine::state::output) {
                throw std::runtime_error{"unexpected state"};
            }
            machine::suspension id = m_machine.step();
            if (id.st != machine::state::output) {
                throw std::runtime_error{"unexpected state"};
            }
            if (x.val == -1 && y.val == 0) {
                m_score = std::max(id.val, m_score);
            }
        }
    }

public:
    cheat_game(std::string_view prog) : m_machine(prog) {
        m_machine.set_addr(0, 2);

        read_outputs();
    }

public:
    word play() {
        try {
            while (true) {
                read_outputs(1);
            }
        }
        catch (won) {
            return m_score;
        }
    }

};

std::unordered_set<std::ptrdiff_t> manual_game::paddle_addresses;
std::unordered_set<std::ptrdiff_t> manual_game::ball_x_addresses;


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: ./a.out PROGRAM\n";
        return 1;
    }

    std::string prog;
    std::fstream f{argv[1]};
    f >> prog;

#if CHEATING
    cheat_game g{prog};
    std::cout << g.play() << '\n';
#else

    for (std::ptrdiff_t ix = 0; ix < 20000; ++ix) {
        manual_game::paddle_addresses.emplace(ix);
        manual_game::ball_x_addresses.emplace(ix);
    }

    while (true) {
        manual_game g{prog};
        g.play();
    }
#endif
}
