#pragma once

#include "relations.hpp"
#include "range.hpp"
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <ostream>
#include <cstddef>

template<typename T> using matrix = std::vector<std::vector<T>>;

template<typename T, typename container = matrix<T>&>
class row_view {
public:
    row_view(container m, const std::size_t row)
        : m(m), row{row < m.size() ? row : throw std::runtime_error{"row out of bounds"}} {}

    T& operator[](const std::size_t col) { return m[row][col]; }
    const T& operator[](const std::size_t col) const { return m[row][col]; }

    std::size_t size() const { return m[row].size(); }

    row_view<T, matrix<T>> operator*(const T rhs) const {
        row_view<T, matrix<T>> result{m, row};
        return result *= rhs;
    }

    template<typename other_container>
    row_view& operator=(const row_view<T, other_container>& rhs) {
        if (size() != rhs.size()) throw std::runtime_error{"rows have different dimensions"};

        for (const auto index : ext::range(0, size())) {
            (*this)[index] = rhs[index];
        }

        return *this;
    }

    template<typename other_container>
    row_view& operator+=(const row_view<T, other_container>& rhs) {
        if (size() != rhs.size()) throw std::runtime_error{"rows have different dimensions"};

        for (const auto index : ext::range(0, size())) {
            (*this)[index] += rhs[index];
        }

        return *this;
    }

    template<typename other_container>
    row_view& operator-=(const row_view<T, other_container>& rhs) {
        if (size() != rhs.size()) throw std::runtime_error{"rows have different dimensions"};

        for (const auto index : ext::range(0, size())) {
            (*this)[index] -= rhs[index];
        }

        return *this;
    }

    template<typename other_container>
    row_view& operator*=(const row_view<T, other_container>& rhs) {
        if (size() != rhs.size()) throw std::runtime_error{"rows have different dimensions"};

        for (const auto index : ext::range(0, size())) {
            (*this)[index] *= rhs[index];
        }

        return *this;
    }

    template<typename other_container>
    row_view& operator/=(const row_view<T, other_container>& rhs) {
        if (size() != rhs.size()) throw std::runtime_error{"rows have different dimensions"};

        for (const auto index : ext::range(0, size())) {
            (*this)[index] /= rhs[index];
        }

        return *this;
    }

    void swap(row_view other) {
        if (size() != other.size()) throw std::runtime_error{"rows have different dimensions"};

        for (const auto index : ext::range(0, size())) {
            std::swap((*this)[index], other[index]);
        }
    }

    row_view& operator=(T rhs) {
        for (const auto index : ext::range(0, size())) {
            (*this)[index] = rhs;
        }

        return *this;
    }

    row_view& operator+=(T rhs) {
        for (const auto index : ext::range(0, size())) {
            (*this)[index] += rhs;
        }

        return *this;
    }

    row_view& operator-=(T rhs) {
        for (const auto index : ext::range(0, size())) {
            (*this)[index] -= rhs;
        }

        return *this;
    }

    row_view& operator*=(T rhs) {
        for (const auto index : ext::range(0, size())) {
            (*this)[index] *= rhs;
        }

        return *this;
    }

    row_view& operator/=(T rhs) {
        for (const auto index : ext::range(0, size())) {
            (*this)[index] /= rhs;
        }

        return *this;
    }

private:
    container m;
    const std::size_t row;
};

namespace std {
    template<typename T>
    void swap(row_view<T> one, row_view<T> another) { one.swap(another); }
}

template<typename T, typename container = matrix<T>&>
class column_view {
public:
    column_view(container m, const std::size_t col)
        : m(m), col{col < m.size() ? col : throw std::runtime_error{"column out of bounds"}} {}

    T& operator[](const std::size_t row) { return m[row][col]; }
    const T& operator[](const std::size_t row) const { return m[row][col]; }

    std::size_t size() const { return m.size(); }

private:
    container m;
    const std::size_t col;
};

template<typename T>
inline matrix<T> reduce_last_row(const matrix<T>& m) {
    if (m.size() < 2) throw std::logic_error{"reduce_last_row on a matrix with < 2 rows"};

    return { std::begin(m), --std::end(m) };
}

template<typename T>
matrix<T> slice(const matrix<T>& m, const std::size_t row_num, const std::size_t col_num,
                const std::size_t row_start = 0, const std::size_t col_start = 0) {
    matrix<T> result{row_num};

    for (const auto i : ext::range(0, row_num)) {
    	result[i].assign(std::begin(m[row_start + i]) + col_start, std::begin(m[row_start + i]) + col_start + col_num);
    }

    return result;
}

template<typename It>
inline void augment_impl(It&, std::size_t) {}
template<typename It, typename Arg, typename... Args>
inline void augment_impl(It& it, const std::size_t i, const matrix<Arg>& arg, const matrix<Args>&... args) {
    std::copy(std::begin(arg[i]), std::end(arg[i]), it);
    augment_impl(it, i, args...);
}

inline bool check_row_num(const std::size_t) { return true; }
template<typename Arg, typename... Args>
inline bool check_row_num(const std::size_t row_num, const matrix<Arg>& arg, const matrix<Args>&... args) {
    return row_num == arg.size() ? check_row_num(row_num, args...) : false;
}

inline std::size_t get_total_length() { return 0; }
template<typename Arg, typename... Args>
inline std::size_t get_total_length(const matrix<Arg>& arg, const matrix<Args>&... args) {
    return arg.front().size() + get_total_length(args...);
}

template<typename Arg, typename... Args>
matrix<typename std::common_type<Arg, Args...>::type> augment(const matrix<Arg>& arg, const matrix<Args>&... args) {
    const auto n = arg.size();
    if (!check_row_num(n, args...)) {
	throw std::runtime_error{"cannot augment matrices with different number of rows"};
    }

    const auto m = get_total_length(arg, args...);
    matrix<typename std::common_type<Arg, Args...>::type> result{n};

    for (const auto i : ext::range(0, n)) {
	auto& row = result[i];
	row.reserve(m);
	auto it = std::back_inserter(row);

	augment_impl(it, i, arg, args...);
    }

    return result;
}

namespace {
    struct throw_on_zero_row {};
    struct continue_on_zero_row {};

    inline void gauss_elimination_handle_zero_row(throw_on_zero_row) {
        throw std::runtime_error{"the matrix is inconsistent"};
    }

    inline void gauss_elimination_handle_zero_row(continue_on_zero_row) {}
}

template<typename T, typename zero_row_policy>
matrix<T> gauss_forward_elimination_impl(matrix<T> m) {
    const auto row_num = m.size();
	const auto col_num = m.front().size();
    if (row_num > col_num) {
        throw std::runtime_error{"the number of columns must be at least the number of rows"};
    }

	std::size_t row_start{};

	for (const auto col : ext::range(0, col_num)) {
        auto non_zero_col = false;

        // try to ensure non-zero element at position `col` of the main diagonal
        if (is_equal(m[row_start][col], T{})) {
            for (const auto row : ext::range(row_start, row_num)) {
                if (!is_equal(m[row][col], T{})) {
                    std::swap(row_view<T>{m, row_start}, row_view<T>{m, row});
                    break;
                }
            }
        }

        const auto el = m[row_start][col];
        if (!is_equal(el, T{})) {
            // normalize row
            row_view<T>{m, row_start} /= el;

            // zero out all the rows below the main diagonal
            for (const auto row : ext::range(row_start + 1, row_num)) {
                const auto el = m[row][col];
                if (is_equal(el, T{})) continue;

                row_view<T>{m, row} -= row_view<T, matrix<T>>{m, row_start} * el;
            }

            if (++row_start == row_num) break;
        } else gauss_elimination_handle_zero_row(zero_row_policy{});
    }

    return std::move(m);
}

template<typename T>
void gauss_backward_elimination_impl(matrix<T>& m) {
    const auto n = m.size();

    for (const auto col : ext::reverse_range(0, n)) {
        for (const auto row : ext::reverse_range(col + 1, n)) {
            const auto el = m[row][col];
            if (is_equal(el, T{})) continue;

            row_view<T>{m, row} -= row_view<T, matrix<T>>{m, col} * el;
        }
    }
}

template<typename T>
inline matrix<T> gauss_elimination(const matrix<T>& m) {
    auto forward_eliminated = gauss_forward_elimination_impl<T, throw_on_zero_row>(m);
    gauss_backward_elimination_impl(forward_eliminated);

    return forward_eliminated;
}

template<typename T>
inline matrix<T> to_echelon(const matrix<T>& m) {
    return gauss_forward_elimination_impl<T, continue_on_zero_row>(m);
}

template<typename T>
matrix<T> identity(const std::size_t n) {
    matrix<T> result{n, std::vector<T>(n)};

    for (const auto i : ext::range(0, n)) result[i][i] = T{1};

    return result;
}

template<typename T>
matrix<T> negate(matrix<T> m) {
    for (auto& row : m) {
        for (auto& item : row) {
            item = -item;
        }
    }

    return m;
}

template<typename T>
matrix<T> transpose(const matrix<T>& mat) {
    const auto m = mat.size(), n = mat.front().size();

    matrix<T> result{n, std::vector<T>(m)};

    for (const auto i : ext::range(0, m)) {
      for (const auto j : ext::range(0, n)) {
            result[j][i] = mat[i][j];
        }
    }

    return result;
}

template<typename T>
matrix<T> invert(const matrix<T>& m) {
    const auto n = m.size();
    return slice(gauss_elimination(augment(m, identity<T>(n))), n, n, 0, n);
}

template<typename T>
matrix<T> operator*(const matrix<T>& lhs, const matrix<T>& rhs) {
    const auto m = lhs.size(), n = lhs.front().size();
    const auto p = rhs.size(), r = rhs.front().size();

    if (n != p) throw std::runtime_error{"inner dimensions of the matrix product operands do not match"};

    matrix<T> result{m, std::vector<T>(r)};
    for (const auto i : ext::range(0, m)) {
        for (const auto j : ext::range(0, r)) {
            for (const auto k : ext::range(0, n)) {
                result[i][j] += lhs[i][k] * rhs[k][j];
            }
        }
    }

    return result;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const matrix<T>& m) {
    os << "[\n";
    for (const auto& row : m) {
	os << '\t';
	for (const auto& elem : row) {
	    os << elem << '\t';
	}

	os << '\n';
    }

    return os << ']' << std::endl;
}
