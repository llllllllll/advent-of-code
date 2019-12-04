#include <array>
#include <iostream>

bool has_duplicate(std::array<char, 6> v) {
    for (std::size_t ix = 0; ix < v.size() - 1; ++ix) {
        if (v[ix + 1] == v[ix]) {
            return true;
        }
    }
    return false;
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
    return has_duplicate(v) && ascending(v);
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
    for (int ix = 0; ix < 6; ++ix) {
        if (lhs[ix] < rhs[ix]) {
            return true;
        }
        else if (lhs[ix] != rhs[ix]) {
            return false;
        }
    }
    return false;
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
