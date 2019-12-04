#include <iostream>
#include <iterator>
#include <numeric>

int fuel(int mass) {
    int out = 0;
    while (mass > 0) {
        int f = std::max(mass / 3 - 2, 0);
        out += f;
        mass = f;
    }
    return out;
}

int main() {
    std::cout << std::accumulate(std::istream_iterator<int>{std::cin},
                                 std::istream_iterator<int>{},
                                 0,
                                 [](int a, int b) { return a + fuel(b); })
              << '\n';
}
