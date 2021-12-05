#include "aoc/aoc.hpp"

class board {
public:
    static constexpr int side = 5;

    board() = default;

    template<std::ranges::input_range R>
    explicit board(R&& r) {
        size_t ix = 0;
        for (const auto& line : r) {
            for_each_regex_match("[0-9]+", line, [&](const auto& match) {
                vals_[ix++] = std::pair{aoc::parse<int>(match.str()), false};
            });
        }
    }

    std::pair<int, bool> operator()(ptrdiff_t r, ptrdiff_t c) const {
        return vals_[r * side + c];
    }

    void set(int cand) {
        for (auto& [i, set] : vals_) {
            if (i == cand) {
                set = true;
                break;
            }
        }
    }

    bool winner() const {
        for (int row = 0; row < side; ++row) {
            bool winning = true;
            for (int col = 0; col < side; ++col) {
                winning &= (*this)(row, col).second;
                if (!winning) {
                    break;
                }
            }
            if (winning) {
                return true;
            }
        }

        for (int col = 0; col < side; ++col) {
            bool winning = true;
            for (int row = 0; row < side; ++row) {
                winning &= (*this)(row, col).second;
                if (!winning) {
                    break;
                }
            }
            if (winning) {
                return true;
            }
        }
        return false;
    }

    int score() const {
        int out = 0;
        for (const auto& [i, set] : vals_) {
            out += i * !set;
        }
        return out;
    }

private:
    friend std::ostream& operator<<(std::ostream& os, const board& b) {
        for (int row = 0; row < side; ++row) {
            for (int col = 0; col < side; ++col) {
                auto [i, set] = b(row, col);
                if (set) {
                    os << '[' << std::setfill(' ') << std::setw(2) << i << ']';
                }
                else {
                    os << ' ' << std::setfill(' ') << std::setw(2) << i << ' ';
                }
            }
            std::cout << '\n';
        }
        return os;
    }

    std::array<std::pair<int, bool>, side * side> vals_;
};

struct state {
    std::vector<int> draws;
    std::vector<board> boards;
};

template<std::ranges::input_range R>
state parse(R&& input) {
    std::vector<aoc::line> lines(input.begin(), input.end());

    state out;
    auto it = lines.begin();
    const auto end = lines.end();
    aoc::for_split(*it, ',', [&](auto cs) {
        out.draws.push_back(aoc::parse<int>(cs));
    });

    ++it;
    while (it != end) {
        out.boards.emplace_back(std::ranges::subrange(it, it + 6));
        it += 6;
    }

    return out;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    state st = parse(input);

    for (int draw : st.draws) {
        for (auto& b : st.boards) {
            b.set(draw);
            if (b.winner()) {
                return b.score() * draw;
            }
        }
    }
    aoc::no_solution();
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
