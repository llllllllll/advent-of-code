#include <array>
#include <charconv>
#include <deque>
#include <iostream>
#include <iterator>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "aoc/aoc.hpp"

enum class opcode {
    nop,
    acc,
    jmp,
};

struct instruction {
    opcode op;
    int param;

    static instruction parse(std::string_view s) {
        using namespace std::literals;

        instruction out;
        auto op = s.substr(0, 3);
        if (op == "nop"sv) {
            out.op = opcode::nop;
        }
        else if (op == "acc"sv) {
            out.op = opcode::acc;
        }
        else if (op == "jmp"sv) {
            out.op = opcode::jmp;
        }
        else {
            throw std::runtime_error{"unknown op"};
        }
        out.param = aoc::parse_integral<int>(s.substr(4));
        return out;
    }
};

class machine {
public:
    explicit machine(std::vector<instruction> ops)
        : ops_(std::move(ops)), ran_(ops_.size()) {}

    bool step() {
        if (ran_[ip_]) {
            return false;
        }
        ran_[ip_] = true;
        auto [op, param] = ops_[ip_];
        switch (op) {
        case opcode::acc:
            acc_ += param;
            [[fallthrough]];
        case opcode::nop:
            ++ip_;
            break;
        case opcode::jmp:
            ip_ += param;
            break;
        default:
            __builtin_unreachable();
        }
        return true;
    }

    void run() {
        while (step())
            ;
    }

    long acc() const {
        return acc_;
    }

private:
    long acc_ = 0;
    std::size_t ip_ = 0;
    std::vector<instruction> ops_;
    std::vector<bool> ran_;
};

template<typename I>
auto parse(I begin, I end) {
    std::vector<instruction> out;
    for (I it = begin; it != end; ++it) {
        out.emplace_back(instruction::parse(it->get()));
    }
    return out;
}

int main() {
    machine m{parse(std::istream_iterator<aoc::line>{std::cin},
                    std::istream_iterator<aoc::line>{})};
    m.run();
    std::cout << m.acc() << '\n';
}
