#include "Quaternion.h"

namespace Zuazo::Math {

template<typename T>
inline Mat4x4<T> toMatrix(const Quaternion<T>& q) noexcept {
	return glm::mat4_cast(q);
}

template<typename T>
inline Vec3<T> getEulerAngles(const Quaternion<T>& q) noexcept {
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