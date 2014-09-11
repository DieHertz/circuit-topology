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

    row_view& operator=(const row_view& rhs) {
        if (size() != rhs.size()) throw std::runtime_error{"rows have different dimensions"};

        for (const auto index : ext::range(0, size())) {
            (*this)[index] = rhs[index];
        }

        return *this;
    }

    row_view& operator+=(const row_view& rhs) {
        if (size() != rhs.size()) throw std::runtime_error{"rows have different dimensions"};

        for (const auto index : ext::range(0, size())) {
            (*this)[index] += rhs[index];
        }

        return *this;
    }

    row_view& operator-=(const row_view& rhs) {
        if (size() != rhs.size()) throw std::runtime_error{"rows have different dimensions"};

        for (const auto index : ext::range(0, size())) {
            (*this)[index] -= rhs[index];
        }

        return *this;
    }

    row_view& operator*=(const row_view& rhs) {
        if (size() != rhs.size()) throw std::runtime_error{"rows have different dimensions"};

        for (const auto index : ext::range(0, size())) {
            (*this)[index] *= rhs[index];
        }

        return *this;
    }

    row_view& operator/=(const row_view& rhs) {
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

template<typename T>
class column_view {
public:
    column_view(matrix<T>& m, const std::size_t col)
        : m(m), col{col < m.size() ? col : throw std::runtime_error{"column out of bounds"}} {}

    T& operator[](const std::size_t row) { return m[row][col]; }
    const T& operator[](const std::size_t row) const { return m[row][col]; }

    std::size_t size() const { return m.size(); }

private:
    matrix<T>& m;
    const std::size_t col;
};

template<typename T>
bool is_zero_col(const std::size_t row_start, const std::size_t col, const matrix<T>& a) {
	for (std::size_t row = row_start, row_num = a.size(); row < row_num; ++row) {
	    if (!is_equal(a[row][col], T{})) return false;
	}

	return true;
}

template<typename T> void copy_col(const matrix<T>& from, const std::size_t col_from,
                                   matrix<T>& to, const std::size_t col_to) {
    const auto row_num = from.size();
    if (row_num != to.size()) {
        throw std::logic_error{"the matrices passed to copy_col have a different number of rows"};
    }

    for (std::size_t row = 0; row < row_num; ++row) {
        to[row][col_to] = from[row][col_from];
    }
}

template<typename T> inline matrix<T> reduce_last_row(const matrix<T>& m) {
    if (m.size() < 2) throw std::logic_error{"reduce_last_row on a matrix with < 2 rows"};

    return { std::begin(m), --std::end(m) };
}

template<typename T>
matrix<T> slice(const matrix<T>& m, const std::size_t row_num, const std::size_t col_num,
                const std::size_t row_start = 0, const std::size_t col_start = 0) {
    matrix<T> result{row_num};

    for (std::size_t i = 0; i < row_num; ++i)
    	result[i].assign(std::begin(m[row_start + i]) + col_start, std::begin(m[row_start + i]) + col_start + col_num);

    return result;
}

template<typename T> matrix<T> to_echelon_matrix(matrix<T> a) {
	std::size_t m{}, n{};

	const auto row_num = a.size();
	const auto col_num = a.front().size();

	while (n < row_num && m < col_num) {
		for (std::size_t col = m; col < col_num; ++col) {
			if (!is_zero_col(n, col, a)) {
				for (std::size_t row = n; row < row_num; ++row) {
					if (a[row][col] != 0) {
						if (row != n) std::swap(a[n], a[row]);
						const auto el = a[n][col];
						std::transform(std::begin(a[n]) + m, std::end(a[n]), std::begin(a[n]) + m, [&] (const T elem) {
							return elem / el;
						});

						break;
					}
				}

				for (std::size_t row = n + 1; row < row_num; ++row) {
					if (is_zero(a[row][col])) continue;

					const auto el = a[row][col];
					for (std::size_t k = col; k < col_num; ++k) {
						a[row][k] -= a[n][k] * el;
					}
				}

				m = col + 1;
				++n;

				break;
			}

			if (col == col_num - 1) return a;
		}
	}

	return a;
}

template<typename T>
matrix<T> identity(const std::size_t n) {
   matrix<T> result{n, std::vector<T>(n)};

    for (std::size_t i = 0; i < n; ++i) result[i][i] = T{1};

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

    for (std::size_t i = 0; i < m; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            result[j][i] = mat[i][j];
        }
    }

    return result;
}

template<typename T>
matrix<T> inverse(matrix<T> m) {
    const auto n = m.size();
    if (n != m.front().size()) throw std::runtime_error{"cannot invert a non-square matrix"};

    auto result = identity<T>(n);

    // forward Gauss elimination
    for (const auto col : ext::range(0, n)) {
        // ensure non-zero element at position `col` of the main diagonal
        if (is_equal(m[col][col], T{})) {
            for (const auto row : ext::range(col + 1, n)) {
                if (!is_equal(m[row][col], T{})) {
                    std::swap(row_view<T>(m, col), row_view<T>(m, row));
                    std::swap(row_view<T>(result, col), row_view<T>(result, row));
                    break;
                }
            }
        }

        // could not find non-zero element
        if (is_equal(m[col][col], T{})) throw std::runtime_error{"the matrix is singular"};

        // zero out all the rows below the main diagonal
        for (const auto row : ext::range(col + 1, n)) {

        }
    }

    return result;
}

template<typename T>
matrix<T> operator*(const matrix<T>& lhs, const matrix<T>& rhs) {
    const auto m = lhs.size(), n = lhs.front().size();
    const auto p = rhs.size(), r = rhs.front().size();

    if (n != p) throw std::runtime_error{"inner dimensions of the matrix product operands do not match"};

    matrix<T> result{m, std::vector<T>(r)};

    for (std::size_t i = 0; i < m; ++i) {
        for (std::size_t j = 0; j < r; ++i) {
            for (std::size_t k = 0; k < n; ++k) {
                result[i][j] = lhs[i][k] * rhs[k][j];
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
