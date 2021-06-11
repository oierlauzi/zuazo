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

inline size_t fromString(std::string_view str, bool& x) noexcept {
	size_t result = 0;

	//These strings need to pe paired in false/true order
	//so that modulus 2 can be applied
	constexpr std::array<std::string_view, 4> strings = {
		"false",
		"true",
		"0",
		"1"
	};

	//Try to make a match
	for(size_t i = 0; i < strings.size() && !result; ++i) {
		const auto& compareTarget = strings[i];
		if(str == compareTarget) {
			//Success!
			result = str.size();
			x = i & 1; //Modulus 2
		}

	}

	return result;
}

inline std::string toString(char x) {
	return std::string({ x });
}

inline size_t fromString(std::string_view str, char& x) noexcept {
	size_t result = 0;

	if(!str.empty()) {
		x = str.front();
		result = 1;
	}

	return result;
}

inline std::string toString(uint8_t x) {
	return std::to_string(x);
}

inline size_t fromString(std::string_view str, uint8_t& x) noexcept {
	const auto last = std::from_chars(str.cbegin(), str.cend(), x).ptr;
	return std::distance(str.cbegin(), last);
}

inline std::string toString(int8_t x) {
	return std::to_string(x);
}

inline size_t fromString(std::string_view str, int8_t& x) noexcept {
	const auto last = std::from_chars(str.cbegin(), str.cend(), x).ptr;
	return std::distance(str.cbegin(), last);
}

inline std::string toString(uint16_t x) {
	return std::to_string(x);
}

inline size_t fromString(std::string_view str, uint16_t& x) noexcept {
	const auto last = std::from_chars(str.cbegin(), str.cend(), x).ptr;
	return std::distance(str.cbegin(), last);
}

inline std::string toString(int16_t x) {
	return std::to_string(x);
}

inline size_t fromString(std::string_view str, int16_t& x) noexcept {
	const auto last = std::from_chars(str.cbegin(), str.cend(), x).ptr;
	return std::distance(str.cbegin(), last);
}

inline std::string toString(uint32_t x) {
	return std::to_string(x);
}

inline size_t fromString(std::string_view str, uint32_t& x) noexcept {
	const auto last = std::from_chars(str.cbegin(), str.cend(), x).ptr;
	return std::distance(str.cbegin(), last);
}

inline std::string toString(int32_t x) {
	return std::to_string(x);
}

inline size_t fromString(std::string_view str, int32_t& x) noexcept {
	const auto last = std::from_chars(str.cbegin(), str.cend(), x).ptr;
	return std::distance(str.cbegin(), last);
}

inline std::string toString(uint64_t x) {
	return std::to_string(x);
}

inline size_t fromString(std::string_view str, uint64_t& x) noexcept {
	const auto last = std::from_chars(str.cbegin(), str.cend(), x).ptr;
	return std::distance(str.cbegin(), last);
}

inline std::string toString(int64_t x) {
	return std::to_string(x);
}

inline size_t fromString(std::string_view str, int64_t& x) noexcept {
	const auto last = std::from_chars(str.cbegin(), str.cend(), x).ptr;
	return std::distance(str.cbegin(), last);
}

inline std::string toString(float x) {
	return std::to_string(x);
}

inline size_t fromString(const std::string& str, float& x) noexcept {
	//TODO Until all major compilers implement from_chars, 
	//use the slower stof
	
	size_t result = 0;
	try {
		x = std::stof(str, &result);
	} catch (const std::invalid_argument&) {
	} catch (const std::out_of_range&) {
	}

	return result;
}

inline size_t fromString(std::string_view str, float& x) noexcept {
	return fromString(std::string(str), x);
}

inline std::string toString(double x) {
	return std::to_string(x);
}

inline size_t fromString(const std::string& str, double& x) noexcept {
	//TODO Until all major compilers implement from_chars, 
	//use the slower stod
	
	size_t result = 0;
	try {
		x = std::stod(str, &result);
	} catch (const std::invalid_argument&) {
	} catch (const std::out_of_range&) {
	}

	return result;
}

inline size_t fromString(std::string_view str, double& x) noexcept {
	return fromString(std::string(str), x);
}



inline const std::string& toString(const std::string& str) {
	return str;
}

inline std::string_view toString(std::string_view str) {
	return str;
}

inline size_t fromString(std::string_view str, std::string& x) noexcept {
	x = str;
	return str.size();
}

inline size_t fromString(std::string_view str, std::string_view& x) noexcept {
	x = str;
	return str.size();
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


template<typename T, typename F>
std::unordered_map<typename std::invoke_result<F, T>::type, T> 
createStringToEnumLUT(F&& toStringFunc) {
	std::unordered_map<typename std::invoke_result<F, T>::type, T> result;

	const auto first = Utils::EnumTraits<T>::first();
	const auto last = Utils::EnumTraits<T>::last();
	for(auto i = first; i <= last; ++i) {
		result.emplace(toStringFunc(i), i);
	}

	return result;
}

template<typename T>
size_t enumFromString(std::string_view str, T& e) {
	//Statically store a map that relates strings to enumerations
	static const auto lut = createStringToEnumLUT<T>(
		[] (const auto& e) -> auto {
			return toString(e);
		}
	);

	size_t result = 0;

	//Try to match the provided string
	const auto ite = lut.find(str);
	if(ite != lut.cend()) {
		result = str.size();
		e = ite->second;
	}

	return result;
}


template<typename T, typename F>
size_t enumFromString(std::string_view str, T& e, F&& toStringFunc) {
	size_t result = 0;

	const auto first = Utils::EnumTraits<T>::first();
	const auto last = Utils::EnumTraits<T>::last();
	for(auto i = first; i <= last && !result; ++i) {
		const auto iAsString = toStringFunc(i);
		if(toStringFunc(i) == str) {
			//Found a match
			result = str.size();
			e = i;
		}
	}

	return result;
}



template<typename T>
inline std::ostream& quote(std::ostream& os, T&& x) {
    return os << '\"' << std::forward<T>(x) << '\"';
}



inline std::string_view removeLeadingBlankCharacters(std::string_view str) noexcept {
	while(!str.empty() && std::isspace(str.front())) {
		str.remove_prefix(1);
	}

	return str;
}

inline std::string_view removeTrailingBlankCharacters(std::string_view str) noexcept {
	while(!str.empty() && std::isspace(str.back())) {
		str.remove_suffix(1);
	}

	return str;
}

inline std::string_view removeBlankCharacters(std::string_view str) noexcept {
	return removeLeadingBlankCharacters(removeTrailingBlankCharacters(str));
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