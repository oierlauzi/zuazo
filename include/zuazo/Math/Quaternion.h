#pragma once

#include "Vector.h"
#include "Matrix.h"

#include <glm/gtc/quaternion.hpp>

namespace Zuazo::Math {

template<typename T>
class Quaternion {
public:
	using value_type = T;

	constexpr Quaternion(const Quaternion& other) = default;
	constexpr Quaternion(Quaternion&& other) = default;

	constexpr Quaternion(	value_type w = value_type(1), 
							value_type x = value_type(0), 
							value_type y = value_type(0), 
							value_type z = value_type(0) );
	template<typename Q>
	constexpr Quaternion(const Quaternion<Q>& other);
	template<typename Q>
	constexpr Quaternion(value_type w, const Vec<Q, 3>& xyz);

	~Quaternion() = default;
	
	constexpr Quaternion&			operator=(const Quaternion& other) = default;
	constexpr Quaternion&			operator=(Quaternion&& other) = default;

	template<typename Q>
	constexpr Quaternion&			operator=(const Quaternion<Q>& other);

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

	value_type w, x, y, z;

};

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;


template<typename T>
constexpr bool operator==(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept;
template<typename T>
constexpr bool operator!=(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept;

template<typename T>
constexpr Quaternion<T> operator+(const Quaternion<T>& a);
template<typename T>
constexpr Quaternion<T> operator-(const Quaternion<T>& a);

template<typename T>
constexpr Quaternion<T> operator+(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept;
template<typename T>
constexpr Quaternion<T> operator-(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept;
template<typename T>
constexpr Quaternion<T> operator*(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept;
template<typename T>
constexpr Quaternion<T> operator*(const Quaternion<T>& lhs, const typename Quaternion<T>::value_type& rhs) noexcept;
template<typename T>
constexpr Quaternion<T> operator*(const typename Quaternion<T>::value_type& lhs, const Quaternion<T>& rhs) noexcept;
template<typename T>
constexpr Quaternion<T> operator/(const Quaternion<T>& lhs, const typename Quaternion<T>::value_type& rhs) noexcept;

template<typename T>
constexpr Quaternion<T>& operator+=(Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept;
template<typename T>
constexpr Quaternion<T>& operator-=(Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept;
template<typename T>
constexpr Quaternion<T>& operator*=(Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept;
template<typename T>
constexpr Quaternion<T>& operator*=(Quaternion<T>& lhs, const typename Quaternion<T>::value_type& rhs) noexcept;
template<typename T>
constexpr Quaternion<T>& operator/=(Quaternion<T>& lhs, const typename Quaternion<T>::value_type& rhs) noexcept;



template<typename T>
constexpr Mat3x3<T> toMatrix(const Quaternion<T>& q) noexcept;

template<typename T>
constexpr Quaternion<T> fromMatrix(const Mat3x3<T>& m) noexcept;


template<typename T>
constexpr Quaternion<typename Vec3<T>::value_type> fromEulerAngles(const Vec3<T>& angles) noexcept;

template<typename T>
constexpr Quaternion<T> fromEulerAngles(const T& yaw, const T& pitch, const T& roll) noexcept;

template<typename T>
constexpr Vec3<typename Quaternion<T>::value_type> toEulerAngles(const Quaternion<T>& q) noexcept;

template<typename T>
constexpr typename Quaternion<T>::value_type getYaw(const Quaternion<T>& q) noexcept;

template<typename T>
constexpr typename Quaternion<T>::value_type getPitch(const Quaternion<T>& q) noexcept;

template<typename T>
constexpr typename Quaternion<T>::value_type getRoll(const Quaternion<T>& q) noexcept;

}

#include "Quaternion.inl"