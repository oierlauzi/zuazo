#include "Quaternion.h"

namespace Zuazo::Math {

template<typename T>
inline Mat4x4<T> toMatrix(const Quaternion<T>& q) noexcept {
	return glm::mat4_cast(q);
}

template<typename T>
inline Quaternion<T> fromEulerAngles(const Vec3<T>& angles) noexcept {
	return fromEulerAngles(angles.z, angles.y, angles.x); //Yaw (Z), Pitch (Y), Roll (X)
}

template<typename T>
inline Quaternion<T> fromEulerAngles(const T& yaw, const T& pitch, const T& roll) noexcept {
	//Based on:
	//https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	constexpr T half = T(0.5);
	const T	cy = Math::cos(yaw * half),
    		sy = Math::sin(yaw * half),
    		cp = Math::cos(pitch * half),
    		sp = Math::sin(pitch * half),
    		cr = Math::cos(roll * half),
    		sr = Math::sin(roll * half);

	return Quaternion<T>(
		cr * cp * cy + sr * sp * sy,
		sr * cp * cy - cr * sp * sy,
		cr * sp * cy + sr * cp * sy,
		cr * cp * sy - sr * sp * cy
	);
}

template<typename T>
inline Vec3<T> toEulerAngles(const Quaternion<T>& q) noexcept {
	return glm::eulerAngles(q);
}

template<typename T>
inline T getYaw(const Quaternion<T>& q) noexcept {
	return glm::yaw(q);
}

template<typename T>
inline T getPitch(const Quaternion<T>& q) noexcept {
	return glm::pitch(q);
}

template<typename T>
inline T getRoll(const Quaternion<T>& q) noexcept {
	return glm::roll(q);
}

}