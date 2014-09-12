#pragma once
#include <type_traits>
#include <utility>
#include <iterator>

/** \brief numeric range iterator, used to represent half-closed interval [begin, end)
	In conjunction with std::reverse_iterator allows for forward and backward iteration
	over corresponding interval.
	Designed to used in combination with ext::range helper function template, example:
		for (const auto n : ext::range(0, 10)) print(n);
*/
namespace ext {
	template<typename T> struct range_iterator {
		T current;

		T& operator*() { return current; }

		range_iterator& operator++() { return ++current, *this; }
		range_iterator& operator--() { return --current, *this; }

		bool operator==(const range_iterator& other) { return current == other.current; }
		bool operator!=(const range_iterator& other) { return current != other.current; }
	};
} /* namespace ext */

namespace std {
	template<> template<typename T>
	struct iterator_traits<ext::range_iterator<T>> {
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = T;
		using difference_type = T;
		using pointer = T*;
		using reference = T&;
	};
} /* namespace std */

namespace ext {
	template<typename T> using reverse_range_iterator = std::reverse_iterator<range_iterator<T>>;

	template<typename T, bool forward> struct range_wrapper {
		using value_type = typename std::remove_reference<T>::type;
		using range_iterator_t = range_iterator<value_type>;
		using iterator = typename std::conditional<forward,
			range_iterator_t,
			reverse_range_iterator<value_type>>::type;

		value_type begin_, end_;

		iterator begin() { return iterator(range_iterator_t{begin_}); }
		iterator end() { return iterator(range_iterator_t{end_}); }
	};

	template<typename T1, typename T2>
	range_wrapper<typename std::common_type<T1, T2>::type, true> range(T1 begin, T2 end) {
		using common_type = typename std::common_type<T1, T2>::type;
		return { static_cast<common_type>(begin), static_cast<common_type>(end) };
	}

	template<typename T1, typename T2>
	range_wrapper<typename std::common_type<T1, T2>::type, false> reverse_range(T1 begin, T2 end) {
		using common_type = typename std::common_type<T1, T2>::type;
		return { static_cast<common_type>(end), static_cast<common_type>(begin) };
	}
} /* namespace ext */
