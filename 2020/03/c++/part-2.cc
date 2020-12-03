#include <array>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

std::size_t trees_hit(const std::vector<std::string>& map, int x_step, int y_step) {
    std::size_t count = 0;
    std::size_t col = 0;
    std::size_t row = 0;
    for (; row < map.size(); row += y_step, col = (col + x_step) % map[0].size()) {
        count += map[row][col] == '#';
    }
    return count;
}

int main() {
    std::vector<std::string> lines{std::istream_iterator<std::string>{std::cin},
                                   std::istream_iterator<std::string>{}};

    std::array<std::size_t, 5> opts = {trees_hit(lines, 1, 1),
                                       trees_hit(lines, 3, 1),
                                       trees_hit(lines, 5, 1),
                                       trees_hit(lines, 7, 1),
                                       trees_hit(lines, 1, 2)};
    std::cout << std::accumulate(opts.begin(),
                                 opts.end(),
                                 1ul,
                                 [](std::size_t a, std::size_t b) { return a * b; })
              << '\n';
}
