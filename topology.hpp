#pragma once

#include "matrix.hpp"
#include "circuit.hpp"

/// @todo assert no closed loop on edges
template<typename T = int> matrix<T> to_incidence_matrix(const circuit& c) {
    const auto node_num = count_nodes(c);
    const auto circuit_size = c.size();
    matrix<T> incidence{node_num, std::vector<T>(circuit_size)};

    for (std::size_t col = 0; col < circuit_size; ++col) {
        incidence[c[col].tail][col] = 1;
        incidence[c[col].head][col] = -1;
    }

    return incidence;
}

template<typename T = float> circuit select_spanning_tree(const circuit& c) {
    const auto reduced_incidence = reduce_last_row(to_incidence_matrix<T>(c));
    const auto echelon = to_echelon_matrix(reduced_incidence);

    const auto row_num = echelon.size();
    const auto col_num = c.size();
    circuit result{col_num};

    std::size_t t{}, l{row_num}, row{};
    for (std::size_t col = 0; col < col_num; ++col) {
        if (row < row_num && is_equal<T>(echelon[row][col], 1)) {
            result[t++] = c[col];
            ++row;
        } else {
            result[l++] = c[col];
        }
    }

    return result;
}
