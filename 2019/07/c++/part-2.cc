#include <cmath>
#include <condition_variable>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <vector>

enum class opcode : int {
    add = 1,
    mul = 2,
    inp = 3,
    outp = 4,
    jit = 5,
    jif = 6,
    lt = 7,
    eq = 8,
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
    std::function<int()> m_input;
    std::function<void(int)> m_output;

public:
    machine() = default;
    machine(const machine&) = default;

    machine(std::istream& prog,
            std::function<int()> input = [] { int out; std::cin >> out; return out; },
            std::function<void(int)> output = [](int n) { std::cout << n << '\n'; })
        : m_ip(0), m_input(input), m_output(output) {
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

    machine(const machine& m, std::function<int()> input, std::function<void(int)> output)
        : m_ip(0), m_prog(m.m_prog), m_input(input), m_output(output) {}


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
            deref(m_ip + 1) = m_input();
            m_ip += 2;
            break;
        case opcode::outp:
            m_output(access(instr.m1, m_ip + 1));
            m_ip += 2;
            break;
        case opcode::jit:
            if (access(instr.m1, m_ip + 1)) {
                m_ip = access(instr.m2, m_ip + 2);
            }
            else {
                m_ip += 3;
            }
            break;
        case opcode::jif:
            if (!access(instr.m1, m_ip + 1)) {
                m_ip = access(instr.m2, m_ip + 2);
            }
            else {
                m_ip += 3;
            }
            break;
        case opcode::lt:
            deref(m_ip + 3) = access(instr.m1, m_ip + 1) < access(instr.m2, m_ip + 2);
            m_ip += 4;
            break;
        case opcode::eq:
            deref(m_ip + 3) = access(instr.m1, m_ip + 1) == access(instr.m2, m_ip + 2);
            m_ip += 4;
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

class channel {
    std::mutex m_lock;
    std::condition_variable m_cond;
    std::deque<int> m_queue;

public:
    void push(int v) {
        std::lock_guard g(m_lock);
        m_queue.push_back(v);
        m_cond.notify_one();
    }

    int pop() {
        std::unique_lock l(m_lock);
        while(m_queue.empty()) {
            m_cond.wait(l);
        }
        int out = m_queue.front();
        m_queue.pop_front();
        return out;
    }
};


class amp_circuit {
    std::vector<int> m_settings;
    std::vector<channel> m_channels;
    std::vector<machine> m_machines;

public:
    amp_circuit(const machine& prog, const std::vector<int>& settings)
        : m_settings(settings),
          m_channels(settings.size()),
          m_machines(settings.size()) {
        if (settings.size() == 0) {
            throw std::invalid_argument{"must have at least one amplifier in a circuit"};
        }

        for (std::size_t ix = 0; ix < settings.size(); ++ix) {
            m_machines[ix] = machine{prog,
                                     [ix, this] { return m_channels[ix].pop(); },
                                     [ix, this](int n) {
                                         m_channels[(ix + 1) % m_channels.size()].push(n);
                                     }};
        }
    }

    int run(int starting_power = 0) {
        for (std::size_t ix = 0; ix < m_settings.size(); ++ix) {
            m_channels[ix].push(m_settings[ix]);
        }
        m_channels[0].push(starting_power);

        std::vector<std::thread> threads;
        for (machine& m : m_machines) {
            threads.emplace_back([](machine* m) { m->run(); }, &m);
        }
        for (auto& t : threads) {
            t.join();
        }

        return m_channels.front().pop();
    }
};

template<typename... Ts>
bool uniq(const Ts&... ts) {
    std::unordered_set<int> s = {ts...};
    return s.size() == sizeof...(ts);
}

int max_power(const machine& prog) {
    int max = -1;
    for (int a = 5; a < 10; ++a) {
        for (int b = 5; b < 10; ++b) {
            for (int c = 5; c < 10; ++c) {
                for (int d = 5; d < 10; ++d) {
                    for (int e = 5; e < 10; ++e) {
                        if (!uniq(a, b, c, d, e)) {
                            continue;
                        }
                        amp_circuit amp{prog, {a, b, c, d, e}};
                        int power = amp.run();
                        if (power > max) {
                            max = power;
                        }
                    }
                }
            }
        }
    }
    return max;
}

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
    machine base(program);

    std::cout << max_power(base) << '\n';
}
