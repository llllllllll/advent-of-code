#include <iostream>
#include <iterator>
#include <optional>
#include <vector>

std::optional<std::pair<int, int>> find_sum_matching(const std::vector<int>& vs,
                                                     int target) {
    for (auto a = vs.begin(); a != vs.end(); ++a) {
        for (auto b = std::next(a); b != vs.end(); ++b) {
            if (*a + *b == target) {
                return std::pair{*a, *b};
            }
        }
    }
    return {};
}

int solution(const std::vector<int>& vs) {
    auto [a, b] = *find_sum_matching(vs, 2020);
    return a * b;
}

int main() {
    std::vector<int> vs{std::istream_iterator<int>{std::cin},
                        std::istream_iterator<int>{}};

    std::cout << solution(vs) << '\n';
}
