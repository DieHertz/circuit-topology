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

/// @note we currently assume that every branch is either voltage-defined or current-defined
bool is_voltage_defined(const element_type& type) {
   static const std::map<element_type, bool> map{
    	{ element_type::voltage_source, true },
    	{ element_type::capacitor, false },
    	{ element_type::resistor, true },
    	{ element_type::inductor, true },
    	{ element_type::current_source, false }
    };

    const auto it = map.find(type);
    return it != std::end(map) ? it->second : throw std::logic_error{"invalid element_type value"};
}

inline bool is_current_defined(const element_type& type) {
    return !is_voltage_defined(type);
}

bool is_source(const element_type& type) {
   static const std::map<element_type, bool> map{
    	{ element_type::voltage_source, true },
    	{ element_type::capacitor, false },
    	{ element_type::resistor, false },
    	{ element_type::inductor, false },
    	{ element_type::current_source, true }
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

    bool is_voltage_defined() const { return ::is_voltage_defined(type); }
    bool is_current_defined() const { return ::is_current_defined(type); }
    bool is_source() const { return ::is_source(type); }

    std::string get_branch_constitutive_equation() const {
        switch (type) {
        case element_type::voltage_source: return "U(" + name + ") = " + name;
    	case element_type::capacitor: return "I(" + name + ") = " + name + " * dU(" + name + ")/dt";
    	case element_type::resistor: return "U(" + name + ") = " + name + " * I(" + name + ")";
    	case element_type::inductor: return "U(" + name + ") = " + name + " * dI(" + name + ")/dt";
    	case element_type::current_source: return "I(" + name + ") = " + name;
        default: throw std::logic_error{"invalid element_type value"};
        };
    }
};

inline bool operator<(const element& lhs, const element& rhs) {
    return lhs.type < rhs.type;
}
