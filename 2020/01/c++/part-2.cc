#include <iostream>
#include <iterator>
#include <optional>
#include <tuple>
#include <vector>

std::optional<std::tuple<int, int, int>> find_sum_matching(const std::vector<int> vs,
                                                           int target) {
    for (auto a = vs.begin(); a != vs.end(); ++a) {
        for (auto b = std::next(a); b != vs.end(); ++b) {
            for (auto c = std::next(b); c != vs.end(); ++c) {
                if (*a + *b + *c == target) {
                    return std::tuple{*a, *b, *c};
                }
            }
        }
    }
    return {};
}

int solution(const std::vector<int>& vs) {
    auto [a, b, c] = *find_sum_matching(vs, 2020);
    return a * b * c;
}

int main() {
    std::vector<int> vs{std::istream_iterator<int>{std::cin},
                        std::istream_iterator<int>{}};

    std::cout << solution(vs) << '\n';
}
