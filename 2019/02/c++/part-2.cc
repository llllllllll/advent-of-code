#include <iostream>
#include <vector>

enum class opcode : int {
    add = 1,
    mul = 2,
    halt = 99,
};

class machine {
private:
    std::size_t m_ip;
    std::vector<int> m_instrs;

public:
    machine(std::istream& in) : m_ip(0) {
        int val = 0;

        char c;
        while (in.get(c)) {
            if (c == ',') {
                m_instrs.emplace_back(val);
                val = 0;
            }
            else if (c >= '0' && c <= '9') {
                val *= 10;
                val += c - '0';
            }
        }
        m_instrs.emplace_back(val);
    }

private:
    int& deref(std::size_t ix) {
        return m_instrs[m_instrs[ix]];
    }

    const int& deref(std::size_t ix) const {
        return m_instrs[m_instrs[ix]];
    }

    class halt {};

    void step() {
        switch(static_cast<opcode>(m_instrs[m_ip])) {
        case opcode::add:
            deref(m_ip + 3) = deref(m_ip + 1) + deref(m_ip + 2);
            m_ip += 4;
            break;
        case opcode::mul:
            deref(m_ip + 3) = deref(m_ip + 1) * deref(m_ip + 2);
            m_ip += 4;
            break;
        case opcode::halt:
            throw halt{};
        default:
            __builtin_unreachable();
        }
    }

public:
    int run(int a, int b) {
        m_instrs[1] = a;
        m_instrs[2] = b;

        try {
            while (true) {
                step();
            }
        }
        catch(halt) {
        }
        return m_instrs[0];
    }

    std::vector<int> instrs() {
        return m_instrs;
    }
};

int main() {
    machine m(std::cin);
    for (int a = 0; a < 100; ++a) {
        for (int b = 0; b < 100; ++b) {
            if (machine{m}.run(a, b) == 19690720) {
                std::cout << 100 * a + b << '\n';
                return 0;
            }
        }
    }
}
