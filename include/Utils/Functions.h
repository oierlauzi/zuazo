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



template<typename T>
constexpr const T& min(const T& a, const T& b);

template<typename T>
constexpr T min(std::initializer_list<T> ilist);

template<typename T>
constexpr const T& max(const T& a, const T& b);

template<typename T>
constexpr T max(std::initializer_list<T> ilist);

template<typename T>
constexpr const T& clamp(const T& val, const T& min, const T& max);

template<typename T>
constexpr bool isInRange(const T& val, const T& min, const T& max);



template<typename T>
std::string	toString(const T& a);


template<typename Func, typename Tuple1, typename... Tuplen>
constexpr auto elementwiseOperation(Func&& op, Tuple1&& first, Tuplen&&... rest);

}

#include "Functions.inl"