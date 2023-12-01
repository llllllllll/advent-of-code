#include <array>
#include <iostream>
#include <iterator>
#include <numeric>
#include <regex>
#include <stdexcept>
#include <string_view>

struct password_line {
    std::array<int, 2> locs;
    char req;
    std::string password;

    password_line() = default;

    password_line(const std::string& line) {
        static const std::regex p{R"((\d+)-(\d+) (.): (.*))"};

        std::smatch match;
        if (!std::regex_match(line, match, p)) {
            throw std::runtime_error{"bad parse"};
        }

        locs[0] = std::stoi(match[1].str()) - 1;
        locs[1] = std::stoi(match[2].str()) - 1;
        req = match[3].str()[0];
        password = match[4];
    }

    password_line(const password_line&) = default;
    password_line& operator=(const password_line&) = default;

    bool valid() const {
        return (password[locs[0]] == req) + (password[locs[1]] == req) == 1;
    }

    friend std::istream& operator>>(std::istream& in, password_line& line) {
        std::string text;
        if (std::getline(in, text)) {
            line = password_line{text};
        }
        return in;
    }
};

template<typename T>
std::size_t count_valid(T begin, T end) {
    return std::accumulate(begin, end, 0ul, [](auto a, auto b) { return a + b.valid(); });
}

int main() {
    std::cout << count_valid(std::istream_iterator<password_line>{std::cin},
                             std::istream_iterator<password_line>{})
              << '\n';
}
