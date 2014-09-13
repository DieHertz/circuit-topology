#pragma once

#include "element.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include <ostream>
#include <cstddef>

using circuit = std::vector<element>;

std::ostream& operator<<(std::ostream& os, const circuit& c) {
    os << "{\n";
    for (const auto& el : c) {
        os << "\t{ " <<
            to_string(el.type) << ", " <<
            el.tail << ", " <<
            el.head << ", " <<
            el.name << " }\n";
    }

    return os << '}' << std::endl;
}

inline std::size_t count_nodes(const circuit& c) {
   return std::accumulate(std::begin(c), std::end(c), std::size_t{},
        [] (const std::size_t node_max, const element& el) {
        return std::max(node_max, std::max(el.tail, el.head));
    }) + 1;
}

circuit normalize(circuit c) {
    std::sort(std::begin(c), std::end(c));
    return c;
}
