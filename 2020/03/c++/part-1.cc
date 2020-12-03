#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

std::size_t trees_hit(const std::vector<std::string>& map) {
    std::size_t count = 0;
    std::size_t col = 0;
    for (const std::string& row : map) {
        count += row[col] == '#';
        col = (col + 3) % row.size();
    }
    return count;
}

int main() {
    std::vector<std::string> lines{std::istream_iterator<std::string>{std::cin},
                                   std::istream_iterator<std::string>{}};
    std::cout << trees_hit(lines) << '\n';
}
