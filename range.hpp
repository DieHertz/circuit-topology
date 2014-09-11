#pragma once
#include <type_traits>
#include <utility>

namespace ext {

template<typename T> struct range_iterator {
	T pos;

	T& operator*() { return pos; }

	range_iterator& operator++() { return ++pos, *this; }
	bool operator!=(const range_iterator& other) { return pos != other.pos; }
};

template<typename T> struct range_wrapper {
	using value_type = typename std::remove_reference<T>::type;
	using iterator = range_iterator<value_type>;

	value_type begin_, end_;

	iterator begin() { return { begin_ }; }
	iterator end() { return { end_ }; }
};

template<typename T1, typename T2> range_wrapper<typename std::common_type<T1, T2>::type> range(T1 begin, T2 end) {
	using common_type = typename std::common_type<T1, T2>::type;
	return { static_cast<common_type>(begin), static_cast<common_type>(end) };
}

} /* namespace ext */
