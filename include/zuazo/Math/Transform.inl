#include "Transform.h"

#include "Transformations.h"

namespace Zuazo::Math {

template<typename T>
Transform<T>::Transform(const Vec3<T>& position,
						const Quaternion<T>& rotation,
						const Vec3<T>& scale,
						const Vec3<T>& center ) noexcept
	: m_rotation(rotation)
	, m_center(center)
	, m_scale(scale)
	, m_position(position)
{
}



template<typename T>
void Transform<T>::setPosition(const Vec3<T>& position) noexcept {
	m_position = position;
}

template<typename T>
const Vec3<T>& Transform<T>::getPosition() const noexcept {
	return m_position;
}


template<typename T>
void Transform<T>::setRotation(const Quaternion<T>& rotation) noexcept {
	m_rotation = rotation;
}

template<typename T>
const Quaternion<T>& Transform<T>::getRotation() const noexcept {
	return m_rotation;
}


template<typename T>
void Transform<T>::setScale(const Vec3<T>& scale) noexcept {
	m_scale = scale;
}


template<typename T>
const Vec3<T>& Transform<T>::getScale() const noexcept {
	return m_position;
}


template<typename T>
void Transform<T>::setCenter(const Vec3<T>& center) noexcept {
	m_center = center;
}

template<typename T>
const Vec3<T>& Transform<T>::getCenter() const noexcept {
	return m_center;
}


template<typename T>
void Transform<T>::lookAt(const Vec3<T>& position, const Vec3<T>& target, const Vec3<T>& up) noexcept {
	setPosition(position);
	lookAt(target, up);
}

template<typename T>
void Transform<T>::lookAt(const Vec3<T>& target, const Vec3<T>& up) noexcept {
	const auto direction = target - getPosition();
	setRotation(Math::lookAt(direction, up));
}


template<typename T>
Math::Mat4x4<T> Transform<T>::calculateMatrix() const noexcept {
	Math::Mat4x4<T> result(1.0f); //Load identity

	result = Math::translate(result, -m_center);
	result = Math::scale(result, m_scale);
	result = Math::rotate(result, m_rotation);
	result = Math::translate(result, m_position);

	return result;
}

}