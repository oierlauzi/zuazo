#include "StringConversions.h"

#include <sstream>
#include <charconv>
#include <cstdlib>

namespace Zuazo {
    	
template<typename T>
inline std::string toString(const T& x) {
	std::stringstream ss;

	ss << x;

	return ss.str();
}


constexpr std::string_view toString(bool x) {
	return x ? "true" : "false";
}

inline bool fromString(std::string_view str, bool& x) noexcept {
	bool result;

	if(str == "false" || str == "0") {
		x = false;
		result = true;
	} else if(str == "true" || str == "1") {
		x = true;
		result = true;
	} else {
		//No match
		result = false;
	}

	return result;
}

inline std::string toString(uint8_t x) {
	return std::to_string(x);
}

inline bool fromString(std::string_view str, uint8_t& x) noexcept {
	return std::from_chars(str.cbegin(), str.cend(), x).ec == std::errc();
}

inline std::string toString(int8_t x) {
	return std::to_string(x);
}

inline bool fromString(std::string_view str, int8_t& x) noexcept {
	return std::from_chars(str.cbegin(), str.cend(), x).ec == std::errc();
}

inline std::string toString(uint16_t x) {
	return std::to_string(x);
}

inline bool fromString(std::string_view str, uint16_t& x) noexcept {
	return std::from_chars(str.cbegin(), str.cend(), x).ec == std::errc();
}

inline std::string toString(int16_t x) {
	return std::to_string(x);
}

inline bool fromString(std::string_view str, int16_t& x) noexcept {
	return std::from_chars(str.cbegin(), str.cend(), x).ec == std::errc();
}

inline std::string toString(uint32_t x) {
	return std::to_string(x);
}

inline bool fromString(std::string_view str, uint32_t& x) noexcept {
	return std::from_chars(str.cbegin(), str.cend(), x).ec == std::errc();
}

inline std::string toString(int32_t x) {
	return std::to_string(x);
}

inline bool fromString(std::string_view str, int32_t& x) noexcept {
	return std::from_chars(str.cbegin(), str.cend(), x).ec == std::errc();
}

inline std::string toString(uint64_t x) {
	return std::to_string(x);
}

inline bool fromString(std::string_view str, uint64_t& x) noexcept {
	return std::from_chars(str.cbegin(), str.cend(), x).ec == std::errc();
}

inline std::string toString(int64_t x) {
	return std::to_string(x);
}

inline bool fromString(std::string_view str, int64_t& x) noexcept {
	return std::from_chars(str.cbegin(), str.cend(), x).ec == std::errc();
}

inline std::string toString(float x) {
	return std::to_string(x);
}

inline bool fromString(const std::string& str, float& x) noexcept {
	//TODO Until all major compilers implement from_chars, 
	//use the slower stof

	bool success;
	try {
		x = std::stof(str);
		success = true;
	} catch (const std::invalid_argument&) {
		success = false;
	} catch (const std::out_of_range&) {
		success = false;
	}

	return success;
}

inline bool fromString(std::string_view str, float& x) noexcept {
	return fromString(std::string(str), x);
}

inline std::string toString(double x) {
	return std::to_string(x);
}

inline bool fromString(const std::string& str, double& x) noexcept {
	//TODO Until all major compilers implement from_chars, 
	//use the slower stod
	
	bool success;
	try {
		x = std::stod(str);
		success = true;
	} catch (const std::invalid_argument&) {
		success = false;
	} catch (const std::out_of_range&) {
		success = false;
	}

	return success;
}

inline bool fromString(std::string_view str, double& x) noexcept {
	return fromString(std::string(str), x);
}



inline const std::string& toString(const std::string& str) {
	return str;
}

inline std::string_view toString(std::string_view str) {
	return str;
}

inline bool fromString(std::string_view str, std::string& x) noexcept {
	x = str;
	return true;
}

inline bool fromString(std::string_view str, std::string_view& x) noexcept {
	x = str;
	return true;	
}



inline std::string_view toString(const std::type_info& tinfo) noexcept {
	return tinfo.name();
}

inline std::ostream& operator<<(std::ostream& os, const std::type_info& tinfo) {
	return os << toString(tinfo);
}

inline std::string_view toString(std::type_index tid) noexcept {
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



//Forward declare EnumTraits
namespace Utils {
template<typename T>
struct EnumTraits;
}

template<typename T>
inline std::unordered_map<std::string_view, T> createStringToEnumLUT() {
	std::unordered_map<std::string_view, T> result;

	const auto first = Utils::EnumTraits<T>::first();
	const auto last = Utils::EnumTraits<T>::last();
	for(auto i = first; i != last; ++i) {
		result.emplace(toString(i), i);
	}

	return result;
}

template<typename T>
inline bool enumFromString(std::string_view str, T& e) {
	static const auto LUT = createStringToEnumLUT<T>();

	const auto ite = LUT.find(str);
	const bool result = ite != LUT.cend();
	if(result) {
		e = ite->second;
	}
	return result;
}



template<typename T>
inline std::ostream& quote(std::ostream& os, T&& x) {
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