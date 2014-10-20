#include "analysis.hpp"
#include "circuit_from_stream.hpp"
#include <iostream>
#include <fstream>
#include <typeinfo>

int main(int argc, char** argv) try {
    if (argc < 2) {
        throw std::runtime_error{"expected circuit file name as second argument"};
    }

    std::ifstream in{argv[1]};
    if (!in) {
        throw std::runtime_error{"could not open file " + std::string{argv[1]}};
    }

    const auto c = circuit_from_stream(in);
    const analysis<float> nodal_analyzer{c};

    std::cout << nodal_analyzer.get_model_equations() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "exception of type " << typeid(e).name() << ": " << e.what() << std::endl;
} catch (...) {
    std::cerr << "unknown exception" << std::endl;
    throw;
}
