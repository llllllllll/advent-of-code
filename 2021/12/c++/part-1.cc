#include "aoc/aoc.hpp"

class node {
public:
    explicit node(std::string name) : name_{std::move(name)} {}

    const std::string& name() const { return name_; }
    bool big() const { return std::isupper(name_[0]); }

    void connect(std::shared_ptr<node> child) { children_.push_back(std::move(child)); }

    template <typename F>
    void visit(F&& f) const {
        for (const auto& child : children_) {
            f(*child);
        }
    }

private:
    std::string name_;
    std::vector<std::shared_ptr<node>> children_;
};

size_t search(const node& node, std::unordered_set<std::string> seen) {
    seen.insert(node.name());
    int out = 0;
    node.visit([&](const auto& child) {
        if (child.name() == "end"sv) {
            ++out;
            return;
        }
        if (seen.count(child.name()) == 0 || child.big()) {
            out += search(child, seen);
        }

    });
    return out;
}

template<std::ranges::input_range R>
auto solution(R&& input) {
    std::unordered_map<std::string, std::shared_ptr<node>> nodes;
    auto get_node = [&](const std::string& name) -> std::shared_ptr<node>& {
        auto [it, inserted] = nodes.try_emplace(name);
        if (inserted) {
            it->second = std::make_shared<node>(name);
        }
        return it->second;
    };
    for (const std::string_view line : input) {
        const auto split = aoc::split(line, '-');
        auto& a = get_node(split[0]);
        auto& b = get_node(split[1]);
        a->connect(b);
        b->connect(a);
    }

    return search(*get_node("start"), {});
}

int main() { aoc::dbg(solution(aoc::input_range<aoc::line>())); }
