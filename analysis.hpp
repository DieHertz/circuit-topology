#pragma once

#include "circuit.hpp"
#include "matrix.hpp"
#include "topology.hpp"
#include <vector>
#include <ostream>

struct equation_term {
    std::string expr;
    bool sign;
};

using equation = std::vector<equation_term>;

std::ostream& operator<<(std::ostream& os, const equation& equation) {
    auto first = true;
    for (const auto & term : equation) {
        os << (first ? term.sign ? "-" : "" : term.sign ? " - " : " + ") << term.expr;
        first = false;
    }
    os << " = 0";

    return os;
}

using equations = std::vector<equation>;

std::ostream& operator<<(std::ostream& os, const equations& equations) {
    for (const auto& equation : equations) {
        os << equation << '\n';
    }

    return os;
}

struct system_of_equations {
    std::vector<std::string> unknowns;
    equation equations;
};

template<typename T> class analysis {
public:
    analysis(const circuit& cir)
        : circuit{select_spanning_tree(normalize(cir))}
        , incidence{reduce_last_row(to_incidence(circuit))}
        , node_num{incidence.size()}, branch_num{circuit.size()}
        , incidence_tree{slice(incidence, node_num, node_num)}
        , incidence_links{slice(incidence, node_num, branch_num - node_num, 0, node_num)}
        , b{augment(-transpose((invert(incidence_tree) * incidence_links)), identity<int>(branch_num - node_num))}
        , d{augment(identity<int>(node_num), -transpose(slice(b, branch_num - node_num, node_num)))}
    {}

    equations get_kcl_equations() const { return matrix_to_equations(d, "I"); }
    equations get_kvl_equations() const { return matrix_to_equations(b, "U"); }

private:
    equations matrix_to_equations(const matrix<int>& m, const std::string& symbol) const {
        equations result{m.size()};

        for (const auto i : ext::range(0, m.size())) {
            for (const auto branch : ext::range(0, branch_num)) {
                const auto el = m[i][branch];
                if (el != 0) result[i].push_back({ symbol + '(' + circuit[branch].name + ')', el < 0 });
            }
        }

        return result;
    }

    const circuit circuit;
    const matrix<int> incidence;
    const std::size_t node_num;
    const std::size_t branch_num;
    const matrix<int> incidence_tree;
    const matrix<int> incidence_links;
    const matrix<int> b;
    const matrix<int> d;
};
