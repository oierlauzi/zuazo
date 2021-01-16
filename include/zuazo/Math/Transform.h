#pragma once

#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

namespace Zuazo::Math {

template <typename T>
class Transform {
public:
	explicit constexpr Transform(	const Vec3<T>& position 		= Vec3<T>(0.0f, 0.0f, 0.0f),
									const Quaternion<T>& rotation	= Quaternion<T>(0.0f, 0.0f, 0.0f, 0.0f),
									const Vec3<T>& scale			= Vec3<T>(1.0f, 1.0f, 1.0f) ) noexcept;
	constexpr Transform(const Transform& other) = default;
	~Transform() = default;

	Transform& 						operator=(const Transform& other) = default;

	constexpr bool					operator==(const Transform& other) const;
	constexpr bool					operator!=(const Transform& other) const;

	constexpr void					setPosition(const Vec3<T>& position) noexcept;
	constexpr const Vec3<T>&		getPosition() const noexcept;

	constexpr void					setRotation(const Quaternion<T>& rotation) noexcept;
	constexpr const Quaternion<T>&	getRotation() const noexcept;

	constexpr void					setScale(const Vec3<T>& scale) noexcept;
	constexpr const Vec3<T>&		getScale() const noexcept;

	constexpr void					lookAt(const Vec3<T>& position, const Vec3<T>& target, const Vec3<T>& up) noexcept;
	constexpr void					lookAt(const Vec3<T>& target, const Vec3<T>& up) noexcept;

	Math::Mat4x4<T>					calculateMatrix() const noexcept;

private:
	Quaternion<T>					m_rotation;
	Vec3<T>							m_scale;
	Vec3<T>							m_position;

};

using Transformf = Transform<float>;
using Transformd = Transform<double>;

}

#include "Transform.inl"