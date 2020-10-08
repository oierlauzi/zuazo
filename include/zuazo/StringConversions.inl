#include "StringConversions.h"

#include <sstream>

namespace Zuazo {
    	
template<typename T>
inline std::string toString(const T& x) {
	std::stringstream ss;

	ss << x;

	return ss.str();
}

inline std::string toString(uint8_t x) {
	return std::to_string(x);
}

inline std::string toString(int8_t x) {
	return std::to_string(x);
}

inline std::string toString(uint16_t x) {
	return std::to_string(x);
}

inline std::string toString(int16_t x) {
	return std::to_string(x);
}

inline std::string toString(uint32_t x) {
	return std::to_string(x);
}

inline std::string toString(int32_t x) {
	return std::to_string(x);
}

inline std::string toString(uint64_t x) {
	return std::to_string(x);
}

inline std::string toString(int64_t x) {
	return std::to_string(x);
}

inline std::string toString(float x) {
	return std::to_string(x);
}

inline std::string toString(double x) {
	return std::to_string(x);
}




inline std::string_view toString(const std::type_info& tinfo) {
	return tinfo.name();
}

inline std::ostream& operator<<(std::ostream& os, const std::type_info& tinfo) {
	return os << toString(tinfo);
}

inline std::string_view toString(std::type_index tid) {
	return tid.name();
}

inline std::ostream& operator<<(std::ostream& os, std::type_index tid) {
	return os << toString(tid);
}





template <typename T1, typename T2>
inline std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& pair) {
	return os << pair.first << ": " << pair.second;
}


template <typename... Types>
inline std::ostream& operator<<(std::ostream& os, const std::tuple<Types...>& tuple) {
	return std::apply(
		[&os](const Types&... elements) -> std::ostream& {
			return printAsTuple(os, elements...);
        },
		tuple
	);
}


template<typename T>
inline std::ostream& enquote(std::ostream& os, T&& x) {
    return os << '\"' << std::forward<T>(x) << '\"';
}

template<typename... Types>
inline std::ostream& printAsTuple(std::ostream& os, const Types&... elements) {
	//Based on:
	//https://en.cppreference.com/w/cpp/utility/apply

	os << '{';

	size_t i{0};
	((os << elements << (++i != sizeof...(Types) ? ", " : "")), ...); //Add a comma only if it is not the last one

	os << '}';

	return os;
}

template<typename InputIt>
inline std::ostream& toOstream(std::ostream& os, InputIt begin, InputIt end) {
	os << '[';
	while(begin != end) {
		os << *begin;

		if(++begin != end) os << ", "; //Add a comma only if it is not the last one
	}
	os << ']';

	return os;
}

}