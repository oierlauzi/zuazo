#pragma once

#include <type_traits>
#include <endian.h>
#include <limits.h>

#define ZUAZO_MACRO_EXPAND(x) (x)
#define ZUAZO_TO_STRING(x) #x

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

#ifdef NDEBUG
	#define ZUAZO_IS_DEBUG (false)
#else
	#define ZUAZO_IS_DEBUG (true)
#endif

#define ZUAZO_CPP_VER (__cplusplus)
#define ZUAZO_IS_CPP11 (ZUAZO_CPP_VER >= 201103L)
#define ZUAZO_IS_CPP14 (ZUAZO_CPP_VER >= 201402L)
#define ZUAZO_IS_CPP17 (ZUAZO_CPP_VER >= 201703L)


#define ZUAZO_ENUM_BIT_OPERATORS(T)																																					\
	constexpr T operator~ (T a) { return static_cast<T>( ~static_cast<std::underlying_type<T>::type>(a) ); }																		\
	constexpr T operator| (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) | static_cast<std::underlying_type<T>::type>(b) ); }					\
	constexpr T operator& (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) & static_cast<std::underlying_type<T>::type>(b) ); }					\
	constexpr T operator^ (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) ^ static_cast<std::underlying_type<T>::type>(b) ); }					\
	inline T& operator|= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) |= static_cast<std::underlying_type<T>::type>(b) ); }		\
	inline T& operator&= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) &= static_cast<std::underlying_type<T>::type>(b) ); }		\
	inline T& operator^= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) ^= static_cast<std::underlying_type<T>::type>(b) ); }

#define ZUAZO_ENUM_ARITHMETIC_OPERATORS(T)																																			\
	constexpr T operator- (T a) { return static_cast<T>( -static_cast<std::underlying_type<T>::type>(a) ); }																		\
	constexpr T operator- (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) - static_cast<std::underlying_type<T>::type>(b) ); }					\
	constexpr T operator+ (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) + static_cast<std::underlying_type<T>::type>(b) ); }					\
	inline T& operator-= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) -= static_cast<std::underlying_type<T>::type>(b) ); }		\
	inline T& operator-- (T& a) { return reinterpret_cast<T&>( --reinterpret_cast<std::underlying_type<T>::type&>(a) ); }															\
	inline T operator-- (T& a, int) { return static_cast<T>( reinterpret_cast<std::underlying_type<T>::type&>(a)-- ); }																\
	inline T& operator+= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) += static_cast<std::underlying_type<T>::type>(b) ); }		\
	inline T& operator++ (T& a) { return reinterpret_cast<T&>( ++reinterpret_cast<std::underlying_type<T>::type&>(a) ); }															\
	inline T operator++ (T& a, int) { return static_cast<T>( reinterpret_cast<std::underlying_type<T>::type&>(a)++ ); }

#define ZUAZO_ENUM_COMP_OPERATORS(T)																																				\
	constexpr bool operator< (T a, T b) { return static_cast<std::underlying_type<T>::type>(a) < static_cast<std::underlying_type<T>::type>(b); }									\
	constexpr bool operator> (T a, T b) { return static_cast<std::underlying_type<T>::type>(a) > static_cast<std::underlying_type<T>::type>(b); }									\
	constexpr bool operator<= (T a, T b) { return static_cast<std::underlying_type<T>::type>(a) <= static_cast<std::underlying_type<T>::type>(b); }									\
	constexpr bool operator>= (T a, T b) { return static_cast<std::underlying_type<T>::type>(a) >= static_cast<std::underlying_type<T>::type>(b); }

#define ZUAZO_DEFAULT_ASSIGMENT_OPERATORS(T)	\
	T& operator=(const T& other) = default; 	\
	T& operator=(T&& other) = default;

#define ZUAZO_ENUM2STR_CASE(ns, e) case ns::e: return ZUAZO_TO_STRING(e);
