#pragma once

#include <type_traits>
#include <string>
#include <memory>
#include <utility>

namespace Zuazo::Utils {
	
template<typename... T>
constexpr void ignore(const T&...);

template<typename T>
constexpr T bit(T pos);

template<typename T, typename Q>
constexpr T align(T ptr, Q alignment);


template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
makeUnique(Args&&... args);

//TODO unique ptr for unbounded array

template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, std::shared_ptr<T>>::type
makeShared(Args&&... args);

//TODO shared ptr for unbounded array


}

#include "Functions.inl"