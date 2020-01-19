#pragma once

#include <type_traits>
#include <endian.h>
#include <limits.h>

#define ZUAZO_MACRO_EXPAND(x) x
#define ZUAZO_TO_STRING(x) #x

#define ZUAZO_BIT(x) (1 << (x))

#define ZUAZO_IGNORE_PARAM(x) (void)(x)

#define ZUAZO_IS_BIG_ENDIAN (BYTE_ORDER == BIG_ENDIAN)
#define ZUAZO_IS_LITTLE_ENDIAN (BYTE_ORDER == LITTLE_ENDIAN)
#define ZUAZO_IS_32BIT (__WORDSIZE == 32)
#define ZUAZO_IS_64BIT (__WORDSIZE == 64)
#define ZUAZO_BYTE_SIZE (CHAR_BIT)

#if ZUAZO_IS_BIG_ENDIAN
	#define ZUAZO_BE_LE(be, le)	(be)
#elif ZUAZO_IS_LITTLE_ENDIAN
	#define ZUAZO_BE_LE(be, le)	(le)
#else
	#define ZUAZO_BE_LE(be, le)	({})
#endif


#define ZUAZO_ENUM_BIT_OPERATORS(T)																																					\
	constexpr T operator~ (T a) { return static_cast<T>( ~static_cast<std::underlying_type<T>::type>(a) ); }																		\
	constexpr T operator| (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) | static_cast<std::underlying_type<T>::type>(b) ); }					\
	constexpr T operator& (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) & static_cast<std::underlying_type<T>::type>(b) ); }					\
	constexpr T operator^ (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) ^ static_cast<std::underlying_type<T>::type>(b) ); }					\
	constexpr T& operator|= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) |= static_cast<std::underlying_type<T>::type>(b) ); }	\
	constexpr T& operator&= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) &= static_cast<std::underlying_type<T>::type>(b) ); }	\
	constexpr T& operator^= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) ^= static_cast<std::underlying_type<T>::type>(b) ); }

#define ZUAZO_ENUM_ARITHMETIC_OPERATORS(T)																																			\
	constexpr T operator- (T a) { return static_cast<T>( -static_cast<std::underlying_type<T>::type>(a) ); }																		\
	constexpr T operator- (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) - static_cast<std::underlying_type<T>::type>(b) ); }					\
	constexpr T operator+ (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) + static_cast<std::underlying_type<T>::type>(b) ); }					\
	constexpr T& operator-= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) -= static_cast<std::underlying_type<T>::type>(b) ); }	\
	constexpr T& operator-- (T& a) { return --reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) ); }														\
	constexpr T operator-- (T& a, int) { return static_cast<T>( reinterpret_cast<std::underlying_type<T>::type&>(a)-- ); }															\
	constexpr T& operator+= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) += static_cast<std::underlying_type<T>::type>(b) ); }	\
	constexpr T& operator++ (T& a) { return ++reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) ); }														\
	constexpr T operator++ (T& a, int) { return static_cast<T>( reinterpret_cast<std::underlying_type<T>::type&>(a)++ ); }

#define ZUAZO_ENUM_COMP_OPERATORS(T)																																				\
	constexpr bool operator< (T a, T b) { static_cast<std::underlying_type<T>::type>(a) < static_cast<std::underlying_type<T>::type>(b); }											\
	constexpr bool operator> (T a, T b) { static_cast<std::underlying_type<T>::type>(a) > static_cast<std::underlying_type<T>::type>(b); }											\
	constexpr bool operator<= (T a, T b) { static_cast<std::underlying_type<T>::type>(a) <= static_cast<std::underlying_type<T>::type>(b); }										\
	constexpr bool operator>= (T a, T b) { static_cast<std::underlying_type<T>::type>(a) >= static_cast<std::underlying_type<T>::type>(b); }

#define ZUAZO_ENUM_LUT_ENTRY(x, ...)			\
	[ static_cast<size_t>(x) ] = { __VA_ARGS__ }

#define ZUAZO_DEFAULT_ASSIGMENT_OPERATORS(T)	\
	T& operator=(const T& other) = default; 	\
	T& operator=(T&& other) = default;
