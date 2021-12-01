#include "aoc/aoc.hpp"

class bad_parse : public std::exception {
public:
    bad_parse(std::string msg) : msg_(std::move(msg)) {}

    const char* what() const noexcept override {
        return msg_.data();
    }

private:
    std::string msg_;
};

struct token {
    enum class type : char {
        bop = '+',
        lpn = '(',
        rpn = ')',
        num = 'x',
        end = '.',
    };

    type tp;
    std::string value;
};

class expr {
public:
    virtual ~expr() = default;
    virtual std::int64_t eval() const = 0;
    virtual std::ostream& repr(std::ostream& s) const = 0;
};

class atom : public expr {
public:
    atom(std::int64_t val) : val_{val} {}

    std::int64_t eval() const override {
        return val_;
    }

    std::ostream& repr(std::ostream& s) const override {
        return s << val_;
    }

private:
    std::int64_t val_;
};

using binop_f = std::function<std::int64_t(std::int64_t, std::int64_t)>;

class binop : public expr {
public:
    enum class op : char {
        add = '+',
        mul = '*',
    };

    binop(std::string_view op_str, std::unique_ptr<expr> lhs, std::unique_ptr<expr> rhs)
        : lhs_{std::move(lhs)}, rhs_{std::move(rhs)} {
        if (op_str.size() != 1) {
            throw aoc::formatted_exc<bad_parse>("unexpected op str: ", aoc::repr(op_str));
        }
        if (op_str[0] == static_cast<char>(op::add)) {
            op_ = op::add;
        }
        else if (op_str[0] == static_cast<char>(op::mul)) {
            op_ = op::mul;
        }
        else {
            throw aoc::formatted_exc<bad_parse>("unexpected op str: ", aoc::repr(op_str));
        }
    }

    std::int64_t eval() const override {
        switch (op_) {
        case op::add:
            return lhs_->eval() + rhs_->eval();
        case op::mul:
            return lhs_->eval() * rhs_->eval();
        default:
            __builtin_unreachable();
        }
    }

    std::ostream& repr(std::ostream& s) const override {
        s << '(';
        lhs_->repr(s);
        s << ' ' << static_cast<char>(op_) << ' ';
        rhs_->repr(s);
        s << ')';
        return s;
    }

private:
    op op_;
    std::unique_ptr<expr> lhs_;
    std::unique_ptr<expr> rhs_;
};

template<std::ranges::input_range R>
auto parse(R&& r);

template<std::ranges::input_range R>
class parser {
private:
    friend auto parse<R>(R&&);

    parser(R&& r) : it_(r.begin()), end_(r.end()) {}

    std::unique_ptr<expr> parse() {
        for (token tok = next_token(); tok.tp != token::type::end; tok = next_token()) {
            switch (tok.tp) {
            case token::type::num:
                stack_.emplace_back(
                    std::make_unique<atom>(aoc::parse<std::int64_t>(tok.value)));
                break;
            case token::type::bop: {
                auto next = next_token();
                switch (next.tp) {
                case token::type::num: {
                    std::unique_ptr<expr> lhs = std::move(stack_.back());
                    stack_.pop_back();
                    stack_.emplace_back(std::make_unique<binop>(
                        tok.value,
                        std::move(lhs),
                        std::make_unique<atom>(aoc::parse<std::int64_t>(next.value))));
                    break;
                }
                case token::type::lpn:
                    stack_.emplace_back(parse());
                    break;
                default:
                    throw aoc::formatted_exc<bad_parse>("unexpected token: ",
                                                        aoc::repr(tok.tp),
                                                        ", expected ",
                                                        aoc::repr(token::type::num),
                                                        " or ",
                                                        aoc::repr(token::type::lpn));
                }
                break;
            }
            case token::type::rpn: {
                assert(!stack_.empty());
                std::unique_ptr<expr> tos = std::move(stack_.back());
                stack_.pop_back();
                return tos;
            }
            default:
                throw aoc::formatted_exc<bad_parse>("unexpected token: ",
                                                    aoc::repr(tok.tp),
                                                    ", expected ",
                                                    aoc::repr(token::type::bop),
                                                    ", ",
                                                    aoc::repr(token::type::num),
                                                    ", ",
                                                    aoc::repr(token::type::lpn),
                                                    ", or",
                                                    aoc::repr(token::type::rpn));
            }
        }
        assert(stack_.size() == 1);
        std::unique_ptr<expr> tos = std::move(stack_.back());
        stack_.pop_back();
        return tos;
    }

    token peek_token() const {
        if (it_ == end_) {
            return token{token::type::end, ""};
        }
        return *it_;
    }

    token next_token() {
        token out = peek_token();
        ++it_;
        return out;
    }

    std::vector<std::unique_ptr<expr>> stack_;
    typename R::iterator it_;
    typename R::iterator end_;
};

template<std::ranges::input_range R>
auto parse(R&& r) {
    return parser{std::forward<R>(r)}.parse();
}

auto tokenize(std::string_view input) {
    std::vector<token> out;
    while (!input.empty()) {
        switch (input[0]) {
        case ' ':
            input.remove_prefix(1);
            break;
        case '+':
        case '*':
            out.emplace_back(token::type::bop, std::string{input.substr(0, 1)});
            input.remove_prefix(1);
            break;
        case '(':
            out.emplace_back(token::type::lpn, std::string{input.substr(0, 1)});
            input.remove_prefix(1);
            break;
        case ')':
            out.emplace_back(token::type::rpn, std::string{input.substr(0, 1)});
            input.remove_prefix(1);
            break;
        default: {
            const auto end = input.find_first_not_of("1234567890");
            out.emplace_back(token::type::num, std::string{input.substr(0, end)});
            if (end == std::string::npos) {
                input = "";
            }
            else {
                input.remove_prefix(end);
            }
        }
        }
    }
    return out;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    auto view = input | std::views::transform([](std::string_view line) {
                    auto p = parse(tokenize(line));
                    p->repr(std::cout) << '\n';
                    aoc::dbg(p->eval());
                    return p->eval();
                });
    return std::accumulate(view.begin(), view.end(), 0l);
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
