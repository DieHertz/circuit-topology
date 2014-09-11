#pragma once

#include <cmath>

template<typename T> inline bool is_equal(const T lhs, const T rhs) { return lhs == rhs; }
template<> bool inline is_equal<float>(const float lhs, const float rhs) { return std::abs(lhs - rhs) < 1.0e-6; }
template<> bool inline is_equal<double>(const double lhs, const double rhs) { return std::abs(lhs - rhs) < 1.0e-6; }
template<> bool inline is_equal<long double>(const long double lhs, const long double rhs) { return std::abs(lhs - rhs) < 1.0e-6; }

template<typename T> bool inline is_zero(const T t) { return is_equal(t, T{}); };
