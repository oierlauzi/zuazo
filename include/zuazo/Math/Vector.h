#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>

namespace Zuazo::Math {

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
constexpr Vec<T, N> operator-(const Vec<T, N>& a);

template<typename T, size_t N>
constexpr Vec<T, N> operator+(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> operator-(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> operator*(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> operator*(const Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> operator*(const typename Vec<T, N>::value_type& lhs, const Vec<T, N>& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> operator/(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> operator/(const Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> operator/(const typename Vec<T, N>::value_type& lhs, const Vec<T, N>& rhs) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N>& operator+=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N>& operator-=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N>& operator*=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N>& operator*=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N>& operator/=(Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N>& operator/=(Vec<T, N>& lhs, const typename Vec<T, N>::value_type& rhs) noexcept;

template<typename T, size_t N>
constexpr T dot(const Vec<T, N>& lhs, const Vec<T, N>& rhs) noexcept;
template<typename T>
constexpr Vec<T, 3> cross(const Vec<T, 3>& lhs, const Vec<T, 3>& rhs) noexcept;
template<typename T, size_t N>
constexpr T length(const Vec<T, N>& a) noexcept;
template<typename T, size_t N>
constexpr T length2(const Vec<T, N>& a) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> normalize(const Vec<T, N>& a) noexcept;


template<typename T, size_t N>
constexpr Vec<T, N> abs(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> min(const Vec<T, N>& a, const Vec<T, N>& b) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> max(const Vec<T, N>& a, const Vec<T, N>& b) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> exp(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> exp2(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> exp10(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> log(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> log2(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> log10(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> pow(const Vec<T, N>& base, const Vec<T, N>& power) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> sqrt(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> mod(const Vec<T, N>& num, const Vec<T, N>& den) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> floor(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> trunc(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> ceil(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> round(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> fract(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> cos(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> cosh(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> sin(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> sinh(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> tan(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> tanh(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> acos(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> acosh(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> asin(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> asinh(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> atan(const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> atan(const Vec<T, N>& y, const Vec<T, N>& x) noexcept;
template<typename T, size_t N>
constexpr Vec<T, N> atanh(const Vec<T, N>& x) noexcept;


template<typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const Vec<T, N>& m);

}

#include "Vector.inl"