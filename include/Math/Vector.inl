#include "Vector.h"

#include "../Utils/Functions.h"

namespace Zuazo::Utils {

template<typename T, intmax_t dim>
inline Math::Vec<T, dim> min(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) {
	return glm::min(a, b);
}

template<typename T, intmax_t dim>
inline Math::Vec<T, dim> max(const Math::Vec<T, dim>& a, const Math::Vec<T, dim>& b) {
	return glm::max(a, b);
}

template<typename T, intmax_t dim>
inline Math::Vec<T, dim> clamp(const Math::Vec<T, dim>& val, const Math::Vec<T, dim>& lo, const Math::Vec<T, dim>& hi){
	 return glm::clamp(val, lo, hi);
}


template<typename T, intmax_t dim>
inline std::string toString(const T& v) {
	constexpr size_t size = dim;

	std::string result = "[" + toString(v[0]);

	for(size_t i = 1; i < size; ++i) {
		result += ", ";
		result += toString(v[i]);
	}

	result += "]";

	return result;
}

}