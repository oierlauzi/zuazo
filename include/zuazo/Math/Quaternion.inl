#include "Quaternion.h"

namespace Zuazo::Math {

template<typename T>
inline Mat4x4<T> toMatrix(const Quaternion<T>& q) {
	return glm::mat4_cast(q);
}

template<typename T>
inline Vec3<T> getEulerAngles(const Quaternion<T>& q) {
	return glm::eulerAngles(q);
}

template<typename T>
inline T getYaw(const Quaternion<T>& q) {
	return glm::yaw(q);
}

template<typename T>
inline T getPitch(const Quaternion<T>& q) {
	return glm::pitch(q);
}

template<typename T>
inline T getRoll(const Quaternion<T>& q) {
	return glm::roll(q);
}


template<typename T>
inline Quaternion<T> lookAt(const Vec3<T>& direction, const Vec3<T>& up) {
	return glm::quatLookAt(direction, up);
}

}