#include <iostream>
#include <iterator>
#include <numeric>

long long fuel(long long mass) {
    long long out = 0;
    while (mass > 0) {
        long long f = mass / 3 - 2;
        if (f < 0) {
            f = 0;
        }
        out += f;
        mass = f;
    }
    return out;
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
