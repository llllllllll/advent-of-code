#include <array>
#include <cstring>
#include <iostream>

bool has_pair(std::array<char, 6> v) {
    char curr = v[0];
    char chain_size = 1;
    for (int ix = 1; ix < 6; ++ix) {
        char c = v[ix];
        if (c == curr) {
            chain_size += 1;
        }
        else if (chain_size == 2) {
            return true;
        }
        else {
            chain_size = 1;
            curr = c;
        }
    }
    return chain_size == 2;
}

bool ascending(std::array<char, 6> v) {
    for (std::size_t ix = 0; ix < v.size() - 1; ++ix) {
        if (v[ix + 1] < v[ix]) {
            return false;
        }
    }
    return true;
}

bool valid_password(std::array<char, 6> v) {
    return has_pair(v) && ascending(v);
}

void incr(std::array<char, 6>& v) {
    for (int ix = v.size() - 1; ix >= 0; --ix) {
        if (v[ix] == 9) {
            v[ix] = 0;
        }
        else {
            ++v[ix];
            break;
        }
    }
}

bool lt(std::array<char, 6> lhs, std::array<char, 6> rhs) {
    return std::memcmp(lhs.data(), rhs.data(), lhs.size()) < 0;
}

int num_passwords() {
    std::array<char, 6> digits = {1, 4, 6, 8, 1, 0};

    int count = 0;
    while (lt(digits, {6, 1, 2, 5, 6, 4})) {
        count += valid_password(digits);
        incr(digits);
    }

    return count;
}

int main() {
    std::cout << num_passwords() << '\n';
}
