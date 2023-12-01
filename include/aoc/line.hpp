#pragma once

#include <iostream>
#include <string>

namespace aoc {
class line : public std::string {
public:
    using std::string::string;

    friend std::istream& operator>>(std::istream& in, line& l) {
        std::getline(in, l);
        return in;
    }
};
}  // namespace aoc
