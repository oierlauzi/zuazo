#include "Matrix.h"

namespace Zuazo::Math {

template <typename T, int N, int M>
constexpr Mat<T, M, N> transpose(const Mat<T, N, M>& m) {
	return glm::transpose(m);
}

template <typename T, int N>
constexpr Mat<T, N, N> inv(const Mat<T, N, N>& m) {
	return glm::inverse(m);
}

template <typename T>
constexpr Mat<T, 2, 2> diagonal(const Vec<T, 2>& v) {
	return Mat<T, 2, 2>(
		v.x,	T(0),
		T(0),	v.y
	);
}

template <typename T>
constexpr Mat<T, 3, 3> diagonal(const Vec<T, 3>& v) {
	return Mat<T, 3, 3>(
		v.x,	T(0),	T(0),
		T(0),	v.y,	T(0),
		T(0),	T(0),	v.z
	);
}

template <typename T>
constexpr Mat<T, 4, 4> diagonal(const Vec<T, 4>& v) {
	return Mat<T, 4, 4>(
		v.x,	T(0),	T(0),	T(0),
		T(0),	v.y,	T(0),	T(0),
		T(0),	T(0),	v.z,	T(0),
		T(0),	T(0),	T(0),	v.w
	);
}

}