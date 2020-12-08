#include <functional>
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
    static auto year_range = [&](const auto& s, int start, int stop) {
        static const std::regex p{R"(\d{4})"};
        std::smatch m;
        if (!std::regex_match(s, m, p)) {
            return false;
        }
        int val = std::stoi(m.str());
        return val >= start && val <= stop;
    };

    static const std::unordered_map<std::string, std::function<bool(const std::string&)>>
        required = {{"byr", [](const auto& p) { return year_range(p, 1920, 2002); }},
                    {"iyr", [](const auto& p) { return year_range(p, 2010, 2020); }},
                    {"eyr", [](const auto& p) { return year_range(p, 2020, 2030); }},
                    {"hgt",
                     [](const auto& p) {
                         static const std::regex pat{R"((\d+)(cm|in))"};
                         std::smatch m;
                         if (!std::regex_match(p, m, pat)) {
                             return false;
                         }
                         int val = std::stoi(m[1]);
                         if (m[2].str() == "cm") {
                             return val >= 150 && val <= 193;
                         }
                         return val >= 59 && val <= 76;
                     }},
                    {"hcl",
                     [](const auto& p) {
                         static const std::regex pat{R"(#[0-9a-f]{6})"};
                         std::smatch m;
                         return std::regex_match(p, m, pat);
                     }},
                    {"ecl",
                     [](const auto& p) {
                         using namespace std::literals;

                         static const std::array colors =
                             {"amb"s, "blu"s, "brn"s, "gry"s, "grn"s, "hzl"s, "oth"s};
                         return std::find(colors.begin(), colors.end(), p) !=
                                colors.end();
                     }},
                    {"pid", [](const auto& p) {
                         static const std::regex pat{R"(\d{9})"};
                         std::smatch m;
                         return std::regex_match(p, m, pat);
                     }}};

    for (const auto& [key, validator] : required) {
        auto search = map.find(key);
        if (search == map.end() || !validator(search->second)) {
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
