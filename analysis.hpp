#pragma once

#include "circuit.hpp"
#include "matrix.hpp"
#include "topology.hpp"
#include <vector>
#include <unordered_map>
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
    if (!first) os << " = 0";

    return os;
}

using equations_t = std::vector<equation>;

std::ostream& operator<<(std::ostream& os, const equations_t& equations) {
    for (const auto& equation : equations) {
        os << equation << '\n';
    }

    return os;
}

struct system_of_equations {
    std::vector<std::string> unknowns;
    equations_t equations;
};

std::ostream& operator<<(std::ostream& os, const system_of_equations& system) {
    os << "unknowns: (";

    auto first = true;
    for (const auto& unknown : system.unknowns) {
        os << (first ? "" : ", ") << unknown;
        first = false;
    }

    os << ")^T\n" << system.equations;

    return os;
}

template<typename T> class analysis {
public:
    analysis(const circuit& circuit)
        : cir{select_spanning_tree(normalize(circuit))}
        , incidence{reduce_last_row(to_incidence(cir))}
        , node_num{incidence.size()}, branch_num{cir.size()}
        , incidence_tree{slice(incidence, node_num, node_num)}
        , incidence_links{slice(incidence, node_num, branch_num - node_num, 0, node_num)}
        , b{augment(-transpose((invert(incidence_tree) * incidence_links)), identity<int>(branch_num - node_num))}
        , d{augment(identity<int>(node_num), -transpose(slice(b, branch_num - node_num, node_num)))}
    {}

    equations_t get_kcl_equations() const { return matrix_to_equations(d, "I"); }
    equations_t get_kvl_equations() const { return matrix_to_equations(b, "U"); }

    system_of_equations get_model_equations() const {
        // select unknowns: each node's potential, current through voltage-defined branches
        std::vector<std::string> unknowns{node_num};
        equations_t equations{node_num};
        const auto voltage_potentials = get_voltage_potential_map(incidence, cir);

        for (const auto node : ext::range(0, node_num)) {
            unknowns[node] = "V_" + std::to_string(node);
            for (const auto branch : ext::range(0, branch_num)) {
                const auto el = incidence[node][branch];
                if (el == 0) continue;

                auto& element = cir[branch];
                // leave voltage-defined elements as is
                if (element.is_voltage_defined()) {
                    equations[node].push_back({ "I_" + element.name, el < 0 });
                } else {
                    // express branch voltage in terms of node potentials
                    const auto potential_it = voltage_potentials.find(element.name);

                    if (element.type == element_type::capacitor) {
                        for (const auto& potential : potential_it->second) {
                            equations[node].push_back({
                                element.name + " * d" + potential.expr + "/dt",
                                static_cast<bool>((el < 0) ^ potential.sign)
                            });
                        }
                    } else if (element.type == element_type::resistor) {
                        for (const auto& potential : potential_it->second) {
                            equations[node].push_back({
                                "1 / " + element.name + " * " + potential.expr,
                                static_cast<bool>((el < 0) ^ potential.sign)
                            });
                        }
                    } else if (element.type == element_type::current_source) {
                        equations[node].push_back({ element.name, el < 0 });
                    }
                }
            }
        }

        for (const auto& element : cir) {
            if (element.is_voltage_defined()) {
                unknowns.emplace_back("I_" + element.name);

                const auto potential_it = voltage_potentials.find(element.name);
                if (element.type == element_type::voltage_source) {
                    equations.emplace_back(potential_it->second);
                    equations.back().push_back({ element.name, true });
                } else if (element.type == element_type::inductor) {
                    equations.emplace_back(potential_it->second);
                    equations.back().push_back({ element.name + " * dI_" + element.name + "/dt", true });
                }
            }
        }

        return { std::move(unknowns), std::move(equations) };
    }

private:
    equations_t matrix_to_equations(const matrix<int>& m, const std::string& symbol) const {
        equations_t result{m.size()};

        for (const auto i : ext::range(0, m.size())) {
            for (const auto branch : ext::range(0, branch_num)) {
                const auto el = m[i][branch];
                if (el != 0) result[i].push_back({ symbol + '_' + cir[branch].name, el < 0 });
            }
        }

        return result;
    }

    using voltage_potential_map = std::unordered_map<std::string, equation>;

    static voltage_potential_map get_voltage_potential_map(const matrix<int>& incidence, const circuit& circuit) {
        voltage_potential_map result{};

        for (const auto branch : ext::range(0, circuit.size())) {
            auto& item = result[circuit[branch].name];
            for (const auto node : ext::range(0, incidence.size())) {
                const auto el = incidence[node][branch];
                if (el != 0) item.push_back({ "V_" + std::to_string(node), el < 0 });
            }
        }

        return result;
    }

    const circuit cir;
    const matrix<int> incidence;
    const std::size_t node_num;
    const std::size_t branch_num;
    const matrix<int> incidence_tree;
    const matrix<int> incidence_links;
    const matrix<int> b;
    const matrix<int> d;
};
