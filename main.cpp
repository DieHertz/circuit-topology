#include "analysis.hpp"
#include <iostream>

int main() {
    const circuit c{
        { element_type::resistor, 0, 1, "R" },
        { element_type::inductor, 1, 2, "L1" },
        { element_type::capacitor, 1, 2, "C" },
        { element_type::voltage_source, 2, 0, "E" },
        { element_type::inductor, 1, 2, "L2" }
    };

    const analysis<float> nodal_analyzer{c};
    std::cout << nodal_analyzer.get_model_equations() << std::endl;
}
