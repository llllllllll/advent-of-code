#include <iostream>
#include <iterator>
#include <numeric>

int fuel(int mass) {
    return mass / 3 - 2;
}

int main() {
    std::cout << std::accumulate(std::istream_iterator<int>{std::cin},
                                 std::istream_iterator<int>{},
                                 0,
                                 [](int a, int b) { return a + fuel(b); })
              << '\n';
}
