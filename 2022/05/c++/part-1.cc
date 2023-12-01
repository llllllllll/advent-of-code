#include "aoc/aoc.hpp"

class Machine {
public:
    void move(size_t src, size_t dst, size_t count) {
        auto& src_stack = stacks_.at(src);
        AOC_ASSERT(src_stack.size() >= count);
        for (size_t i = 0; i < count; ++i) {
            stacks_.at(dst).push_back(src_stack.back());
            src_stack.pop_back();
        }
    }

    void push(size_t ix, char c) {
        if (stacks_.size() <= ix) {
            stacks_.resize(ix + 1);
        }
        stacks_[ix].insert(stacks_[ix].begin(), c);
    }

    auto peek(size_t ix) const -> char {
        auto const& stack = stacks_.at(ix);
        AOC_ASSERT(not stack.empty());
        return stack.back();
    }

    auto size() const -> size_t { return stacks_.size(); }

private:
    std::vector<std::vector<char>> stacks_;
};

template<std::ranges::input_range R>
auto solution(R&& input) {
    Machine m;
    auto it = input.begin();
    auto const end = input.end();
    for (; it != end; ++it) {
        auto&& line = *it;
        if (line.empty()) {
            ++it;
            break;
        }

        aoc::for_each_regex_match(R"(\[([A-Z])])", line, [&](auto match) {
            m.push(match.position(0) / 4, match[1].str()[0]);
        });
    }
    for (; it != end; ++it) {
        auto&& line = *it;
        auto const maybe_match =
            aoc::regex_match(R"(move (\d+) from (\d+) to (\d+))", line);
        AOC_ASSERT(maybe_match);
        auto const& match = *maybe_match;
        auto const count = aoc::parse<size_t>(match[1].str()) ;
        auto const src = aoc::parse<size_t>(match[2].str()) - 1;
        auto const dst = aoc::parse<size_t>(match[3].str()) - 1;

        m.move(src, dst, count);
    }

    std::string msg;
    for (size_t ix = 0; ix < m.size(); ++ix) {
        msg.push_back(m.peek(ix));
    }
    return msg;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
