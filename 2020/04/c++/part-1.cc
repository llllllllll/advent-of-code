#include <array>
#include <iostream>
#include <iterator>
#include <numeric>
#include <regex>
#include <unordered_map>

#include "aoc/line.hpp"

using passport = std::unordered_map<std::string, std::string>;

template<typename I>
std::vector<passport> parse_passports(I begin, I end) {
    static const std::regex p{R"(([^ ]+):([^ ]+))"};

    std::vector<passport> out(1);

    for (auto it = begin; it != end; ++it) {
        const std::string& line = *it;
        if (line.empty()) {
            out.emplace_back();
        }
        for (auto it = std::sregex_iterator(line.begin(), line.end(), p);
             it != std::sregex_iterator{};
             ++it) {
            out.back().emplace((*it)[1], (*it)[2]);
        }
    }

    return out;
}

bool is_complete_passport(const std::unordered_map<std::string, std::string>& map) {
    using namespace std::literals;

    static const std::array required =
        {"byr"s, "iyr"s, "eyr"s, "hgt"s, "hcl"s, "ecl"s, "pid"s};

    for (const auto& req : required) {
        if (map.count(req) == 0) {
            return false;
        }
    }
    return true;
}

int main() {
    const auto passports = parse_passports(std::istream_iterator<aoc::line>{std::cin},
                                           std::istream_iterator<aoc::line>{});
    std::cout << std::count_if(passports.begin(), passports.end(), is_complete_passport)
              << '\n';
}
