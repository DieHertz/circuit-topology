#include "topology.hpp"
#include <iostream>

void test() {
   const circuit c{
        { element_type::resistor, 0, 1, "R" },
        { element_type::inductor, 1, 2, "L1" },
        { element_type::capacitor, 1, 2, "C" },
        { element_type::voltage_source, 2, 0, "E" },
        { element_type::inductor, 1, 2, "L2" }
        // { element_type::voltage_source, 0, 3, "E1" },
        // { element_type::capacitor, 0, 1, "C1" },
        // { element_type::capacitor, 1, 3, "C2" },
        // { element_type::capacitor, 4, 0, "C3" },
        // { element_type::inductor, 1, 4, "L1" },
        // { element_type::inductor, 3, 4, "L2" },
        // { element_type::inductor, 1, 2, "L3" },
        // { element_type::inductor, 2, 4, "L4" }
    };

    const auto c_normalized = normalize(c);
    std::cout << "circuit description:\n" << c_normalized << std::endl;

    const auto node_num = count_nodes(c);
    const auto independent_node_num = node_num - 1;
    std::cout << "number of nodes = " << node_num << std::endl;

    const auto branch_num = c.size();
    std::cout << "number of branches = " << branch_num << std::endl;

    const auto incidence_reduced = reduce_last_row(to_incidence_matrix(c_normalized));
	std::cout << "A:\n" << incidence_reduced << std::endl;

	const auto echelon = to_echelon(incidence_reduced);
	std::cout << "A_ech:\n" << echelon << std::endl;

    const auto c_with_tree = select_spanning_tree(c_normalized);
    std::cout << "circuit with spanning tree in left rows:\n" << c_with_tree << std::endl;

    const auto a = reduce_last_row(to_incidence_matrix(c_with_tree));
    std::cout << "A[A_T | A_L]:\n" << a << std::endl;

    const auto a_t = slice(a, independent_node_num, independent_node_num);
    const auto a_l = slice(a, independent_node_num, branch_num - independent_node_num, 0, independent_node_num);

    const auto b_t = transpose(negate(invert(a_t) * a_l));
    const auto b = augment(b_t, identity<int>(b_t.size()));
    std::cout << "B[B_T | 1]:\n" << b << std::endl;

    const auto d_l = transpose(negate(b_t));
    const auto d = augment(identity<int>(d_l.size()), d_l);
    std::cout << "D[1 | D_L]:\n" << d << std::endl;

    const auto pretty_print = [&c_with_tree] (const matrix<int>& m, const std::string& name, const std::string& sym) {
        std::cout << name << ":\n";

        for (const auto row : ext::range(0, m.size())) {
            auto first = true;

            for (const auto col : ext::range(0, m.front().size())) {
                const auto el = m[row][col];
                if (is_equal(el, 0)) continue;

                if (first) {
                    if (is_equal(el, -1)) std::cout << '-';
                } else {
                    std::cout << ' ' << (is_equal(el, 1) ? '+' : '-') << ' ';
                }
                std::cout << sym << '(' << c_with_tree[col].name << ')';

                first = false;
            }

            std::cout << " = 0\n";
        }

        std::cout << std::endl;
    };

    pretty_print(a, "KCL", "I");
    pretty_print(b, "KVL", "V");
}

void book_example() {
    std::cout << "\nExample from book:" << std::endl;
    const matrix<int> a{
        { 1, 1, 0, 0, 0, 0, -1 },
        { -1, 0, 0, 1, -1, 0, 0 },
        { 0, 0, 0, -1, 0, 1, 0 },
        { 0, 0, -1, 0, 1, -1, 1 }
    };
    const auto independent_node_num = a.size();
    const auto branch_num = a.front().size();
    std::cout << "A[A_T | A_L]:\n" << a << std::endl;

    const auto a_t = slice(a, independent_node_num, independent_node_num);
    std::cout << "A_T:\n" << a_t << std::endl;

    const auto a_l = slice(a, independent_node_num, branch_num - independent_node_num, 0, independent_node_num);
    std::cout << "A_L:\n" << a_l << std::endl;

    const auto a_t_inv = invert(a_t);
    std::cout << "inv(A_T):\n" << a_t_inv << std::endl;

    const auto b_t = transpose(negate(invert(a_t) * a_l));
    std::cout << "B_T:\n" << b_t << std::endl;

    const auto b = augment(b_t, identity<int>(b_t.size()));
    std::cout << "B:\n" << b << std::endl;

    const auto d_l = transpose(negate(b_t));
    const auto d = augment(identity<int>(d_l.size()), d_l);
    std::cout << "D:\n" << d << std::endl;
}

int main() {
    test();
    // book_example();
}
