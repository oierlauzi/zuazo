#pragma once

#include "../Utils/Matrix.h"
#include "../Utils/Vector.h"
#include "../Utils/Resolution.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace Zuazo::Graphics::MatrixOperations{

template<typename T>
inline typename Utils::Mat4x4<T> eulerAngle(const Utils::Vec3<T>& angles){
	return glm::eulerAngleYXZ(angles.y, angles.x, angles.z);
}

template<typename T>
inline typename Utils::Mat4x4<T> rotate(const Utils::Mat4x4<T>& matrix, const Utils::Vec3<T>& axis, T angle){
	return glm::rotate(matrix, angle, axis);
}

template<typename T>
inline typename Utils::Mat4x4<T> scale(const Utils::Mat4x4<T>& matrix, const Utils::Vec3<T>& scales){
	return glm::scale(matrix, scales);
}

template<typename T>
inline typename Utils::Mat4x4<T> translate(const Utils::Mat4x4<T>& matrix, const Utils::Vec3<T>& position){
	return glm::translate(matrix, position);
}

template<typename T>
inline typename Utils::Mat4x4<T> lookAt(const Utils::Vec3<T>& camera, const Utils::Vec3<T>& focus, const Utils::Vec3<T>& up = Utils::Vec3<T>(0, 1, 0)){
	return glm::lookAt(camera, focus, up);
}

template<typename T>
inline typename Utils::Mat4x4<T> ortho(const Utils::Resolution& res, T nearClip, T farClip){
	return glm::ortho(
			-(T)res.width / 2,
			(T)res.width / 2,
			-(T)res.height / 2,
			(T)res.height / 2,
			nearClip,
			farClip
	);
}

template<typename T>
inline typename Utils::Mat4x4<T> perspective(const Utils::Resolution& res, T fov, T nearClip, T farClip){
	return glm::perspective(
			fov,
			(T)res.width / (T)res.height,
			nearClip,
			farClip
	);
}




}
