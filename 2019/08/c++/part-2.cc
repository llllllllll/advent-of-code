#include <array>
#include <iostream>
#include <tuple>
#include <vector>

template<std::size_t width, std::size_t height>
struct layer {
    std::array<char, width * height> buf;

    char operator()(std::ptrdiff_t row, std::ptrdiff_t col) const {
        return buf[row * width + col];
    }

    char* data() {
        return buf.data();
    }
};

template<std::size_t width, std::size_t height>
std::vector<layer<width, height>> parse_layers(std::istream& in) {
    std::vector<layer<width, height>> out(1);
    while (in.read(out.back().data(), width * height)) {
        out.emplace_back();
    }
    return out;
}

template<std::size_t width, std::size_t height>
layer<width, height> flatten(const std::vector<layer<width, height>>& layers) {
    layer<width, height> out;

    std::size_t ix = 0;
    for (char& c : out.buf) {
        for (const auto& l : layers) {
            if (l.buf[ix] != '2') {
                c = l.buf[ix];
                break;
            }
        }
        ++ix;
    }

    return out;
}

template<std::size_t width, std::size_t height>
std::string render(const layer<width, height>& l) {
    std::string out;
    for (std::size_t r = 0; r < height; ++r) {
        for (std::size_t c = 0; c < width; ++c) {
            switch (l(r, c)) {
            case '0':
                out += "█";
                break;
            case '1':
                out += ' ';
            }
        }
        out += '\n';
    }
    return out;
}

int main() {
    std::cout << render(flatten(parse_layers<25, 6>(std::cin)));
}
