#include "Transform.h"

#include "Transformations.h"

namespace Zuazo::Math {

template<typename T>
constexpr Transform<T>::Transform(	const Vec3<T>& position,
									const Quaternion<T>& rotation,
									const Vec3<T>& scale ) noexcept
	: m_rotation(rotation)
	, m_scale(scale)
	, m_position(position)
{
}


template<typename T>
constexpr bool Transform<T>::operator==(const Transform& other) const {
	return 	m_rotation == other.m_rotation &&
			m_scale == other.m_scale &&
			m_position == other.m_position ;
}

template<typename T>
constexpr bool Transform<T>::operator!=(const Transform& other) const {
	return !operator==(other);
}


template<typename T>
constexpr void Transform<T>::setPosition(const Vec3<T>& position) noexcept {
	m_position = position;
}

template<typename T>
constexpr const Vec3<T>& Transform<T>::getPosition() const noexcept {
	return m_position;
}


template<typename T>
constexpr void Transform<T>::setRotation(const Quaternion<T>& rotation) noexcept {
	m_rotation = rotation;
}

template<typename T>
constexpr const Quaternion<T>& Transform<T>::getRotation() const noexcept {
	return m_rotation;
}


template<typename T>
constexpr void Transform<T>::setScale(const Vec3<T>& scale) noexcept {
	m_scale = scale;
}

template<typename T>
constexpr const Vec3<T>& Transform<T>::getScale() const noexcept {
	return m_scale;
}


template<typename T>
constexpr void Transform<T>::lookAt(const Vec3<T>& position, const Vec3<T>& target, const Vec3<T>& up) noexcept {
	setPosition(position);
	lookAt(target, up);
}

template<typename T>
constexpr void Transform<T>::lookAt(const Vec3<T>& target, const Vec3<T>& up) noexcept {
	const auto direction = target - getPosition();
	setRotation(Math::lookAt(direction, up));
}


template<typename T>
Math::Mat4x4<T> Transform<T>::calculateMatrix() const noexcept {
	Math::Mat4x4<T> result(1.0f); //Load identity

	result = Math::scale(result, m_scale);
	result = Math::rotate(result, m_rotation);
	result = Math::translate(result, m_position);

	return result;
}

}