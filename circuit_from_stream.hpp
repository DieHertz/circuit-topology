#pragma once

#include "circuit.hpp"
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>

circuit circuit_from_stream(std::istream& is) {
    circuit result{};

    static const std::unordered_map<char, element_type> char_to_element_type{
        { 'E', element_type::voltage_source },
        { 'C', element_type::capacitor },
        { 'R', element_type::resistor },
        { 'L', element_type::inductor },
        { 'I', element_type::current_source }
    };

    std::unordered_set<std::string> element_names{};

    std::string str{};
    for (std::size_t line_num = 1; std::getline(is, str); ++line_num) {
        std::istringstream in{str};

        std::string name;
        if (!(in >> name)) {
            throw std::runtime_error{"expected element name at line " + std::to_string(line_num)};
        }

        if (element_names.count(name) != 0) {
            throw std::runtime_error{"duplicate element name " + name + " at line " + std::to_string(line_num)};
        }

        const auto it = char_to_element_type.find(name[0]);
        if (it == std::end(char_to_element_type)) {
            throw std::runtime_error{"unknown element " + name + " at line " + std::to_string(line_num)};
        }
        const auto type = it->second;

        std::size_t tail, head;
        if (!(in >> tail >> head)) {
            throw std::runtime_error{"expected element tail and head node numbers at line " + std::to_string(line_num)};
        }

        result.push_back({ type, tail, head, name });
        element_names.emplace(std::move(name));
    }

    return result;
}
