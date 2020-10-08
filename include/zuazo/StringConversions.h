#pragma once

#include <string>
#include <ostream>

#include <string_view>
#include <cstddef>
#include <typeindex>
#include <utility>
#include <tuple>

namespace Zuazo {

//Default operators. At least one needs to be specialised
template<typename T>
std::string toString(const T& x);

//Numeric types
std::string	toString(uint8_t x);
std::string	toString(int8_t x);
std::string	toString(uint16_t x);
std::string	toString(int16_t x);
std::string	toString(uint32_t x);
std::string	toString(int32_t x);
std::string	toString(uint64_t x);
std::string	toString(int64_t x);
std::string	toString(float x);
std::string	toString(double x);

//Typeinfo-s
std::string_view toString(const std::type_info& tinfo);
std::ostream& operator<<(std::ostream& os, const std::type_info& tinfo);
std::string_view toString(std::type_index tid);
std::ostream& operator<<(std::ostream& os, std::type_index tid);

//Pair (key and value)
template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& pair);

//Tuple
template <typename... Types>
std::ostream& operator<<(std::ostream& os, const std::tuple<Types...>& tuple);

//Helper functions
template<typename T>
std::ostream& enquote(std::ostream& os, T&& x);

template<typename... Types>
std::ostream& printAsTuple(std::ostream& os, const Types&... elements);

template<typename InputIt>
std::ostream& toOstream(std::ostream& os, InputIt begin, InputIt end);

}

#include "StringConversions.inl"