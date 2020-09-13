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

template<typename T>
T* align(T* ptr, size_t alignment);
constexpr uintptr_t align(uintptr_t ptr, size_t alignment);


template<typename Func, typename... Args>
typename std::enable_if<!std::is_same<Func, typename std::invoke_result<Func, Args...>::type>::value, void>::type
invokeIf(Func&& f, Args&&... args);


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