#include <iostream>
#include <memory>
#include <unordered_map>

std::unordered_map<std::string, std::string> parse_direct_orbits(std::istream& in) {
    std::string base;
    std::string orbiter;
    std::string* target = &base;

    std::unordered_map<std::string, std::string> out;

    char c;
    while (in.get(c)) {
        if (c == '\n') {
            out[orbiter] = base;

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

    return out;
}

int count_orbits(const std::unordered_map<std::string, std::string>& m) {
    int count = 0;
    for (const auto& [k, v ] : m) {
        const std::string* target = &v;
        while (*target != "COM") {
            target = &m.find(*target)->second;
            ++count;
        }
        ++count;
    }
    return count;
}

int main() {
    auto m = parse_direct_orbits(std::cin);
    std::cout << count_orbits(m) << '\n';
}
