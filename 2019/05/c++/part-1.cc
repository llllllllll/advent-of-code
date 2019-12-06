#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

enum class opcode : int {
    add = 1,
    mul = 2,
    inp = 3,
    outp = 4,
    halt = 99,
};

enum class access_mode {
    pos = 0,
    imm = 1,
};

struct instruction {
    opcode op;
    access_mode m1;
    access_mode m2;
    access_mode m3;

private:
    int digit(int n, int digit) {
        return (n % static_cast<int>(std::pow(10, digit))) / std::pow(10, digit - 1);
    }

public:
    instruction(int encoded)
        : op(opcode{encoded % 100}),
          m1(access_mode{digit(encoded, 3)}),
          m2(access_mode{digit(encoded, 4)}),
          m3(access_mode{digit(encoded, 5)}) {}
};

class machine {
private:
    std::size_t m_ip;
    std::vector<int> m_prog;

public:
    machine(std::istream& in) : m_ip(0) {
        int val = 0;
        bool negate = false;

        auto commit = [&] {
            if (negate) {
                val = -val;
            }
            m_prog.emplace_back(val);
            val = 0;
            negate = false;
        };

        char c;
        while (in.get(c)) {
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
    int& deref(std::size_t ix) {
        return m_prog[m_prog[ix]];
    }

    int deref(std::size_t ix) const {
        return m_prog[m_prog[ix]];
    }

    int access(access_mode mode, std::size_t ix) const {
        switch (mode) {
        case access_mode::pos:
            return deref(ix);
        case access_mode::imm:
            return m_prog[ix];
        default:
            __builtin_unreachable();
        }
    }

    class halt {};

    void step() {
        instruction instr{m_prog[m_ip]};
        switch(instr.op) {
        case opcode::add:
            deref(m_ip + 3) = access(instr.m1, m_ip + 1) + access(instr.m2, m_ip + 2);
            m_ip += 4;
            break;
        case opcode::mul:
            deref(m_ip + 3) = access(instr.m1, m_ip + 1) * access(instr.m2, m_ip + 2);
            m_ip += 4;
            break;
        case opcode::inp:
            std::cin >> deref(m_ip + 1);
            m_ip += 2;
            break;
        case opcode::outp:
            std::cout << access(instr.m1, m_ip + 1) << '\n';
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
        catch(halt) {
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
    machine m(program);
    m.run();
}
