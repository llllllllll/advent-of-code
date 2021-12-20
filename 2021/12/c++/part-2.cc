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

std::unordered_set<std::string> search(const node& node,
                                       std::unordered_map<std::string, int> seen,
                                       bool consumed_double_visit,
                                       std::string path) {
    ++seen[node.name()];
    std::unordered_set<std::string> out;
    node.visit([&](const auto& child) {
        std::string next_path = path + "," + child.name();
        if (child.name() == "end"sv) {
            out.emplace(std::move(next_path));
            return;
        }
        if (child.big()) {
            for (auto& n : search(child, seen, consumed_double_visit, next_path)) {
                out.emplace(std::move(n));
            }
            return;
        }
        const auto seen_count = seen[child.name()];
        if (!consumed_double_visit && seen_count == 1) {
            for (auto& n : search(child, seen, true, next_path)) {
                out.emplace(std::move(n));
            }
        }
        else if (seen_count == 0) {
            for (auto& n : search(child, seen, consumed_double_visit, next_path)) {
                out.emplace(std::move(n));
            }
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
        if (a->name() != "start"sv) {
            b->connect(a);
        }
    }

    const auto paths = search(*get_node("start"), {{"start", 1}}, false, "start");
    return paths.size();
}

int main() { aoc::dbg(solution(aoc::input_range<aoc::line>())); }
