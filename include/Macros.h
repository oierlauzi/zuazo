#pragma once

#include <type_traits>
#include <endian.h>

#define ZUAZO_MACRO_EXPAND(x) x
#define ZUAZO_TO_STRING(x) #x

#define ZUAZO_BIT(x) 1 << (x)

#define ZUAZO_IGNORE_PARAM(x) (void)(x)

#define ZUAZO_IS_BIG_ENDIAN (BYTE_ORDER == BIG_ENDIAN)
#define ZUAZO_IS_LITTLE_ENDIAN (BYTE_ORDER == LITTLE_ENDIAN)

#define ZUAZO_ENUM_FLAG_OPERATORS(T)                                                                                                                                            \
	inline T operator~ (T a) { return static_cast<T>( ~static_cast<std::underlying_type<T>::type>(a) ); }                                                                       \
	inline T operator| (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) | static_cast<std::underlying_type<T>::type>(b) ); }                   \
	inline T operator& (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) & static_cast<std::underlying_type<T>::type>(b) ); }                   \
	inline T operator^ (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) ^ static_cast<std::underlying_type<T>::type>(b) ); }                   \
	inline T& operator|= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) |= static_cast<std::underlying_type<T>::type>(b) ); }   \
	inline T& operator&= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) &= static_cast<std::underlying_type<T>::type>(b) ); }   \
	inline T& operator^= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) ^= static_cast<std::underlying_type<T>::type>(b) ); }

#define ZUAZO_DEFAULT_ASSIGMENT_OPERATORS(T)	\
	T& operator=(const T& other) = default; 	\
	T& operator=(T&& other) = default;
