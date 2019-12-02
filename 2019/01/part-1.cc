#include <iostream>
#include <iterator>
#include <numeric>

long long fuel(long long mass) {
    return mass / 3 - 2;
}

int main() {
    long long sum = 0;
    for (std::istream_iterator<double> it(std::cin);
         it != std::istream_iterator<double>();
         ++it) {
        sum += fuel(*it);
    }

    std::cout << sum << '\n';
}
