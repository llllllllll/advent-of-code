#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <ranges>
#include <vector>

#include <aoc/point.hpp>
#include <aoc/repr.hpp>

namespace aoc {
template<typename T>
class matrix {
public:
    matrix(ptrdiff_t rows, ptrdiff_t cols)
        : storage_(rows * cols), rows_{rows}, cols_{cols} {}

    matrix(ptrdiff_t rows, ptrdiff_t cols, const T& init)
        : storage_(rows * cols, init), rows_{rows}, cols_{cols} {}

    T& operator()(ptrdiff_t r, ptrdiff_t c) { return storage_[r * cols_ + c]; }
    const T& operator()(ptrdiff_t r, ptrdiff_t c) const {
        return storage_[r * cols_ + c];
    }

    template<typename U>
    T& operator()(const point<U>& p) {
        return (*this)(p.x(), p.y());
    }
    template<typename U>
    const T& operator()(const point<U>& p) const {
        return (*this)(p.x(), p.y());
    }

    auto flat() const { return std::views::all(storage_); }
    auto flat() { return std::views::all(storage_); }

    ptrdiff_t rows() const { return rows_; }
    ptrdiff_t cols() const { return cols_; }

private:
    friend std::ostream& operator<<(std::ostream& os, const matrix& m) {
        std::vector<size_t> longest_sizes(m.cols());
        matrix<std::string> as_strings(m.rows(), m.cols());
        for (ptrdiff_t row = 0; row < m.rows(); ++row) {
            for (ptrdiff_t col = 0; col < m.cols(); ++col) {
                const auto& str = as_strings(row, col) = repr_str(m(row, col));
                longest_sizes[col] = std::max(longest_sizes[col], str.size());
            }
        }
        for (ptrdiff_t col = 0; col < m.cols(); ++col) {
            const auto max_size = longest_sizes[col];
            for (ptrdiff_t row = 0; row < m.rows(); ++row) {
                auto& str = as_strings(row, col);
                str.insert(0, max_size - str.size(), ' ');
            }
        }

        os << '{';
        ptrdiff_t row = 0;
        ptrdiff_t col = 0;
        for (const auto& val : as_strings.flat()) {
            os << val;
            ++col;
            if (col == m.cols()) {
                col = 0;
                ++row;
                if (row == m.rows()) {
                    os << '}';
                }
                else {
                    os << "\n ";
                }
            }
            else {
                os << ", ";
            }
        }
        return os;
    }

    std::vector<T> storage_;
    ptrdiff_t rows_;
    ptrdiff_t cols_;
};
}  // namespace aoc
