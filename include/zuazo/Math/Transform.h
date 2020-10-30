#pragma once

#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

namespace Zuazo::Math {

template <typename T>
class Transform {
public:
	explicit Transform(	const Vec3<T>& position 		= Vec3<T>(0.0f, 0.0f, 0.0f),
						const Quaternion<T>& rotation	= Quaternion<T>(0.0f, 0.0f, 0.0f, 0.0f),
						const Vec3<T>& scale			= Vec3<T>(1.0f, 1.0f, 1.0f),
						const Vec3<T>& center 			= Vec3<T>(0.0f, 0.0f, 0.0f) ) noexcept;
	Transform(const Transform& other) = default;
	~Transform() = default;

	Transform& 				operator=(const Transform& other) = default;

	void					setPosition(const Vec3<T>& position) noexcept;
	const Vec3<T>&			getPosition() const noexcept;

	void					setRotation(const Quaternion<T>& rotation) noexcept;
	const Quaternion<T>&	getRotation() const noexcept;

	void					setScale(const Vec3<T>& scale) noexcept;
	const Vec3<T>&			getScale() const noexcept;

	void					setCenter(const Vec3<T>& center) noexcept;
	const Vec3<T>&			getCenter() const noexcept;

	void					lookAt(const Vec3<T>& position, const Vec3<T>& target, const Vec3<T>& up) noexcept;
	void					lookAt(const Vec3<T>& target, const Vec3<T>& up) noexcept;

	Math::Mat4x4<T>			calculateMatrix() const noexcept;

private:
	Quaternion<T>			m_rotation;
	Vec3<T>					m_center;
	Vec3<T>					m_scale;
	Vec3<T>					m_position;

};

using Transformf = Transform<glm::f32>;
using Transformd = Transform<glm::f64>;

}

#include "Transform.inl"