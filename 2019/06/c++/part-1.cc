#include <iostream>
#include <unordered_map>
#include <memory>

struct node {
    std::string name;
    std::unordered_map<std::string, std::shared_ptr<node>> connections;

    node(const std::string& name) : name(name) {}
};

std::unordered_map<std::string, std::shared_ptr<node>> parse_orbits(std::istream& in) {
    std::string base;
    std::string orbiter;
    std::string* target = &base;

    std::unordered_map<std::string, std::shared_ptr<node>> nodes;

    char c;
    while (in.get(c)) {
        if (c == '\n') {
            auto& n = nodes[base];
            if (!n) {
                n = std::make_shared<node>(base);
            }
            auto& ptr = nodes[orbiter];
            if (!ptr) {
                ptr = std::make_shared<node>(orbiter);
            }
            n->connections[orbiter] = ptr;
            ptr->connections[base] = n;

            base.clear();
            orbiter.clear();
            target = &base;
        }
        else if (c == ')') {
            target = &orbiter;
        }
        else {
            target->push_back(c);
        }
    }

    return nodes;
}

int shortest_path(const std::unordered_map<std::string, std::shared_ptr<node>>& g,
                  std::shared_ptr<node> start,
                  std::shared_ptr<node> end,
                  std::shared_ptr<node> prev,
                  int distance) {
    if (start == end) {
        return distance;
    }
    int shortest = -1;
    for (const auto& [k, v] : start->connections) {
        if (v == prev) {
            continue;
        }
        int dist = shortest_path(g, v, end, start, distance + 1);
        if (shortest < 0 || (dist >= 0 && dist <= shortest)) {
            shortest = dist;
        }
    }
    return shortest;
}

int shortest_path(const std::unordered_map<std::string, std::shared_ptr<node>>& g,
                  const std::string& start,
                  const std::string& end) {
    auto s = g.find(start);
    if (s == g.end()) {
        throw std::runtime_error{"couldn't find start node"};
    }
    auto e = g.find(end);
    if (e == g.end()) {
        throw std::runtime_error{"couldn't find end node"};
    }

    return shortest_path(g, s->second, e->second, nullptr, 0);
}

int main() {
    auto g = parse_orbits(std::cin);
    std::cout << shortest_path(g, "YOU", "SAN") - 2 << '\n';
}
