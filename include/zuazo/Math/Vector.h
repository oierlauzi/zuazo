#pragma once

#include "../Utils/BufferView.h"

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <type_traits>
#include <functional>

namespace Zuazo::Math {

//Tag used to represent that a normalized vector is provided
struct normalized_t {};
constexpr normalized_t normalized;

template <typename T, size_t N>
class Vec;

template<typename T>
class Vec<T, 2> {
public:
	using value_type = T;

	constexpr explicit Vec(value_type val = value_type(0));
	constexpr Vec(const Vec& other) = default;
	constexpr Vec(Vec&& other) = default;

	constexpr Vec(value_type x, value_type y);
	template<typename Q>
	constexpr Vec(const Vec<Q, 2>& other);

	~Vec() = default;
	
	constexpr Vec&					operator=(const Vec& other) = default;
	constexpr Vec&					operator=(Vec&& other) = default;

	template<typename Q>
	constexpr Vec&					operator=(const Vec<Q, 2>& other);

	constexpr value_type&			operator[](size_t i);
	constexpr const value_type&		operator[](size_t i) const;

	constexpr value_type*			begin();
	constexpr const value_type*		begin() const;
	constexpr const value_type*		cbegin() const;
	constexpr value_type*			end();
	constexpr const value_type*		end() const;
	constexpr const value_type*		cend() const;

	constexpr value_type*			data();
	constexpr const value_type*		data() const;

	static constexpr size_t			size();

	value_type x, y;

};

template<typename T>
class Vec<T, 3> {
public:
	using value_type = T;

	constexpr explicit Vec(value_type val = value_type(0));
	constexpr Vec(const Vec& other) = default;
	constexpr Vec(Vec&& other) = default;

	constexpr Vec(value_type x, value_type y, value_type z = value_type(0));
	template<typename Q>
	constexpr Vec(const Vec<Q, 3>& other);

	template<typename Q>
	constexpr Vec(const Vec<Q, 2>& xy, value_type z = value_type(0));
	template<typename Q>
	constexpr Vec(value_type x, const Vec<Q, 2>& yz);

	~Vec() = default;
	
	constexpr Vec&					operator=(const Vec& other) = default;
	constexpr Vec&					operator=(Vec&& other) = default;

	template<typename Q>
	constexpr Vec&					operator=(const Vec<Q, 2>& other);
	template<typename Q>
	constexpr Vec&					operator=(const Vec<Q, 3>& other);

	constexpr value_type&			operator[](size_t i);
	constexpr const value_type&		operator[](size_t i) const;

	constexpr value_type*			begin();
	constexpr const value_type*		begin() const;
	constexpr const value_type*		cbegin() const;
	constexpr value_type*			end();
	constexpr const value_type*		end() const;
	constexpr const value_type*		cend() const;

	constexpr value_type*			data();
	constexpr const value_type*		data() const;

	static constexpr size_t			size();

	value_type x, y, z;

};

template<typename T>
class Vec<T, 4> {
public:
	using value_type = T;

	constexpr explicit Vec(value_type val = value_type(0));
	constexpr Vec(const Vec& other) = default;
	constexpr Vec(Vec&& other) = default;

	constexpr Vec(value_type x, value_type y, value_type z = value_type(0), value_type w = value_type(1));
	template<typename Q>
	constexpr Vec(const Vec<Q, 4>& other);

	template<typename Q>
	constexpr Vec(const Vec<Q, 3>& xyz, value_type w = value_type(1));
	template<typename Q>
	constexpr Vec(value_type x, const Vec<Q, 3>& yzw);

	template<typename Q>
	constexpr Vec(const Vec<Q, 2>& xy, value_type z = value_type(0), value_type w = value_type(1));
	template<typename Q>
	constexpr Vec(value_type x, const Vec<Q, 2>& yz, value_type w = value_type(1));
	template<typename Q>
	constexpr Vec(value_type x, value_type y, const Vec<Q, 2>& zw);
	template<typename Q, typename R>
	constexpr Vec(const Vec<Q, 2>& xy, const Vec<R, 2>& zw);

	~Vec() = default;
	
	constexpr Vec&					operator=(const Vec& other) = default;
	constexpr Vec&					operator=(Vec&& other) = default;

	template<typename Q>
	constexpr Vec&					operator=(const Vec<Q, 2>& other);
	template<typename Q>
	constexpr Vec&					operator=(const Vec<Q, 3>& other);
	template<typename Q>
	constexpr Vec&					operator=(const Vec<Q, 4>& other);

	constexpr value_type&			operator[](size_t i);
	constexpr const value_type&		operator[](size_t i) const;

	constexpr value_type*			begin();
	constexpr const value_type*		begin() const;
	constexpr const value_type*		cbegin() const;
	constexpr value_type*			end();
	constexpr const value_type*		end() const;
	constexpr const value_type*		cend() const;

	constexpr value_type*			data();
	constexpr const value_type*		data() const;

	static constexpr size_t			size();

	value_type x, y, z, w;

};


template <typename T>
using Vec2 = Vec<T, 2>;

template <typename T>
using Vec3 = Vec<T, 3>;

template <typename T>
using Vec4 = Vec<T, 4>;

using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;

using Vec2d = Vec<double, 2>;
using Vec3d = Vec<double, 3>;
using Vec4d = Vec<double, 4>;

using Vec2i = Vec<int32_t, 2>;
using Vec3i = Vec<int32_t, 3>;
using Vec4i = Vec<int32_t, 4>;

using Vec2u = Vec<uint32_t, 2>;
using Vec3u = Vec<uint32_t, 3>;
using Vec4u = Vec<uint32_t, 4>;



template<typename T, size_t N>
constexpr bool operator==(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;
template<typename T, size_t N>
constexpr bool operator!=(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;


template<typename T, size_t N>
constexpr Vec<T, N> operator+(const Vec<T, N>& a);

template<typename T, size_t N>
constexpr Vec<typename std::invoke_result<std::negate<void>, T>::type, N>
operator-(const Vec<T, N>& a);

template<typename T, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_not<void>, T>::type, N>
operator~(const Vec<T, N>& a);


template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::plus<void>, T, Q>::type, N>
operator+(const Vec<T, N>& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::minus<void>, T, Q>::type, N>
operator-(const Vec<T, N>& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::multiplies<void>, T, Q>::type, N> 
operator*(const Vec<T, N>& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::multiplies<void>, T, Q>::type, N> 
operator*(const Vec<T, N>& lhs, const Q& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::multiplies<void>, T, Q>::type, N> 
operator*(const T& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::divides<void>, T, Q>::type, N> 
operator/(const Vec<T, N>& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::divides<void>, T, Q>::type, N> 
operator/(const Vec<T, N>& lhs, const Q& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::divides<void>, T, Q>::type, N>
operator/(const T& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::modulus<void>, T, Q>::type, N> 
operator%(const Vec<T, N>& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::modulus<void>, T, Q>::type, N> 
operator%(const Vec<T, N>& lhs, const Q& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::modulus<void>, T, Q>::type, N>
operator%(const T& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_and<void>, T, Q>::type, N> 
operator&(const Vec<T, N>& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_and<void>, T, Q>::type, N> 
operator&(const Vec<T, N>& lhs, const Q& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_and<void>, T, Q>::type, N>
operator&(const T& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_or<void>, T, Q>::type, N> 
operator|(const Vec<T, N>& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_or<void>, T, Q>::type, N> 
operator|(const Vec<T, N>& lhs, const Q& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_or<void>, T, Q>::type, N>
operator|(const T& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_xor<void>, T, Q>::type, N> 
operator^(const Vec<T, N>& lhs, const Vec<Q, N>& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_xor<void>, T, Q>::type, N> 
operator^(const Vec<T, N>& lhs, const Q& rhs);

template<typename T, typename Q, size_t N>
constexpr Vec<typename std::invoke_result<std::bit_xor<void>, T, Q>::type, N>
operator^(const T& lhs, const Vec<Q, N>& rhs);

//TODO operator << and >>


template<typename T, size_t N>
constexpr Vec<T, N>& operator+=(Vec<T, N>& lhs, const Vec<T, N>& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator-=(Vec<T, N>& lhs, const Vec<T, N>& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator*=(Vec<T, N>& lhs, const Vec<T, N>& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator*=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator/=(Vec<T, N>& lhs, const Vec<T, N>& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator/=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator%=(Vec<T, N>& lhs, const Vec<T, N>& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator%=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator&=(Vec<T, N>& lhs, const Vec<T, N>& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator&=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator|=(Vec<T, N>& lhs, const Vec<T, N>& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator|=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator^=(Vec<T, N>& lhs, const Vec<T, N>& rhs);
template<typename T, size_t N>
constexpr Vec<T, N>& operator^=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs);

//TODO operator <<= and >>=


template<typename Func, typename... T, size_t N>
constexpr Vec<typename std::invoke_result<Func, const typename Vec<T, N>::value_type&...>::type, N> 
transform(Func f, const Vec<T, N>&... v);

template<typename T, size_t N>
constexpr typename Vec3<T>::value_type dot(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;

template<typename T>
constexpr Vec3<T> cross(const Vec3<T>& lhs, const Vec3<T>& rhs) noexcept;

template<typename T>
constexpr typename Vec3<T>::value_type mixed(const Vec3<T>& v0, const Vec3<T>& v1, const Vec3<T>& v2) noexcept;

template<typename T>
constexpr typename Vec2<T>::value_type zCross(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept;

template<typename T>
constexpr Vec2<T> perp(const Vec2<T>& v) noexcept;


template<typename T, size_t N>
constexpr T length(const Vec<T, N>& a) noexcept;

template<typename T, size_t N>
constexpr T length2(const Vec<T, N>& a) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> normalize(const Vec<T, N>& a) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> proj(const Vec<T, N>& dir, const Vec<T, N>& p) noexcept;


template<typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const Vec<T, N>& m);

}

namespace Zuazo::Utils {

template<typename T, typename H>
struct Hasher;

template <typename T, size_t N, typename H>
struct Hasher<Math::Vec<T, N>, H> {
	using value_type = Math::Vec<T, N>;
	using hash_type = H;

	constexpr hash_type operator()(const value_type& v) const noexcept;
};

}

#include "Vector.inl"