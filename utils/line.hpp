#pragma once

#include <iostream>

namespace aoc {
class line {
public:
    friend std::istream& operator>>(std::istream& in, line& l) {
        std::getline(in, l.data_);
        return in;
    }

    operator const std::string&() const {
        return data_;
    }

    operator std::string&() {
        return data_;
    }

    const std::string& get() const {
        return data_;
    }

    std::string& get() {
        return data_;
    }

private:
    std::string data_;
};
}  // namespace aoc
