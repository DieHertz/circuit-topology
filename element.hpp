#pragma once

#include <map>
#include <ostream>
#include <stdexcept>
#include <cstddef>

enum struct element_type {
	voltage_source,
	capacitor,
	resistor,
	inductor,
	current_source
};

std::string to_string(const element_type& type) {
    static const std::map<element_type, std::string> map{
    	{ element_type::voltage_source, "element_type::voltage_source" },
    	{ element_type::capacitor, "element_type::capacitor" },
    	{ element_type::resistor, "element_type::resistor" },
    	{ element_type::inductor, "element_type::inductor" },
    	{ element_type::current_source, "element_type::current_source" }
    };

    const auto it = map.find(type);
    return it != std::end(map) ? it->second : throw std::logic_error{"invalid element_type value"};
}

std::ostream& operator<<(std::ostream& os, element_type type) {
    return os << to_string(type);
}

struct element {
    element_type type;
    std::size_t tail;
    std::size_t head;
    std::string name;
};

inline bool operator<(const element& lhs, const element& rhs) {
    return lhs.type < rhs.type;
}
