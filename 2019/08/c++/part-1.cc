#include <array>
#include <iostream>
#include <tuple>
#include <vector>

template<std::size_t width, std::size_t height>
using layer = std::array<char, width * height>;

template<std::size_t width, std::size_t height>
std::vector<layer<width, height>> parse_layers(std::istream& in) {
    std::vector<layer<width, height>> out(1);
    std::size_t ptr = 0;
    char c;
    while (in.get(c)) {
        if (ptr == width * height) {
            out.emplace_back();
            ptr = 0;
        }
        out.back()[ptr] = c - '0';
        ++ptr;
    }

    return out;
}

int main() {
    auto layers = parse_layers<25, 6>(std::cin);

    std::vector<std::tuple<unsigned int, int>> counts;
    for (const auto& layer : layers) {
        std::array<unsigned char, 10> counter = {0};
        for (unsigned char c : layer) {
            counter[c] += 1;
        }
        counts.emplace_back(counter[0], counter[1] * counter[2]);
    }

    unsigned int min = -1;
    int answer = -1;
    for (auto [z, a] : counts) {
        if (z < min) {
            answer = a;
            min = z;
        }
    }

    std::cout << answer << '\n';
}
