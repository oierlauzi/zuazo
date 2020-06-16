#pragma once

#include <type_traits>
#include <string>
#include <initializer_list>

namespace Zuazo::Utils {
	
template<typename... T>
constexpr void ignore(const T&...);

template<typename T>
constexpr T bit(T pos);

template<typename T, typename Q>
constexpr T align(T ptr, Q alignment);



template<typename Func, typename Tuple1, typename... Tuplen>
constexpr auto elementwiseOperation(Func&& op, Tuple1&& first, Tuplen&&... rest);

}

namespace Zuazo {
	
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

}

#include "Functions.inl"