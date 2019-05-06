#pragma once

#include "../Utils/Vector.h"
#include "../Utils/Resolution.h"

#include <glm/glm.hpp>

namespace Zuazo::Graphics::VectorOperations{

template<typename T>
inline T distance(const Utils::Vec3<T>& v1, const Utils::Vec3<T>& v2){
	return glm::distance(v1, v2);
}

template<typename T>
inline T length(const Utils::Vec3<T>& vec){
	return glm::length(vec);
}

}
