#include <numeric>
#include <iostream>
#include <regex>
#include <string_view>
#include <stdexcept>

struct password_line {
    int lower;
    int upper;
    char req;
    std::string password;

    password_line() = default;

    password_line(const std::string& line) {
        static const std::regex p{R"((\d+)-(\d+) (.): (.*))"};

        std::smatch match;
        if (!std::regex_match(line, match, p)) {
            throw std::runtime_error{"bad parse"};
        }

        lower = std::stoi(match[1].str());
        upper = std::stoi(match[2].str());
        req = match[3].str()[0];
        password = match[4];
    }

    password_line(const password_line&) = default;
    password_line& operator=(const password_line&) = default;

    bool valid() const {
        int count = std::count(password.begin(), password.end(), req);
        return count >= lower && count <= upper;
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
