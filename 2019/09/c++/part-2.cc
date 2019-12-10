#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
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
            mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
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
private:
    std::size_t m_ip;
    word m_offset_reg;
    memory_space m_prog;
    std::function<word()> m_input;
    std::function<void(word)> m_output;

public:
    machine() = default;

    machine(
        std::istream& prog,
        std::function<word()> input =
            [] {
                word out;
                std::cin >> out;
                return out;
            },
        std::function<void(word)> output = [](word n) { std::cout << n << '\n'; })
        : m_ip(0), m_offset_reg(0), m_prog(1L << 32), m_input(input), m_output(output) {
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

    void inp(instruction instr) {
        waccess(instr.m1, m_ip + 1) = m_input();
        m_ip += 2;
    }

    void outp(instruction instr) {
        m_output(raccess(instr.m1, m_ip + 1));
        m_ip += 2;
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

    struct halt_signal {};

    [[noreturn]] void halt() {
        throw halt_signal{};
    }

    void step() {
        instruction instr = next_instr();
        switch (instr.op) {
        case opcode::add:
            return add(instr);
        case opcode::mul:
            return mul(instr);
        case opcode::inp:
            return inp(instr);
        case opcode::outp:
            return outp(instr);
        case opcode::jit:
            return jit(instr);
        case opcode::jif:
            return jif(instr);
        case opcode::lt:
            return lt(instr);
        case opcode::eq:
            return eq(instr);
        case opcode::rbo:
            return rbo(instr);
        case opcode::halt:
            return halt();
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
        catch (halt_signal) {
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
    machine m(program, [] { return 2; });
    m.run();
}
