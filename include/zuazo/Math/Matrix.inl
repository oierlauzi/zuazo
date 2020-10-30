#include "Matrix.h"

namespace Zuazo::Math {

template <typename T, int N, int M>
inline Mat<T, N, M> inv(const Mat<T, N, M>& m) {
	return glm::inverse(m);
}

}