#pragma once

#include "../Bezier.h"

#include <limits>

namespace Zuazo::Math::LoopBlinn {

enum class CurveType {
	POINT,
	LINE,
	QUADRATIC,
	SERPENTINE,
	CUSP,
	LOOP,
};

template<typename T>
struct Classifier {
	using value_type = T;
	using curve_type = CubicBezier<Vec2<value_type>>;

	struct Result {
		CurveType			type = static_cast<CurveType>(-1);
		value_type			d1 = std::numeric_limits<value_type>::quiet_NaN();
		value_type			d2 = std::numeric_limits<value_type>::quiet_NaN();
		value_type			d3 = std::numeric_limits<value_type>::quiet_NaN();
		value_type			discriminantTerm1 = std::numeric_limits<value_type>::quiet_NaN();
	};

	constexpr Result operator()(const curve_type& curve) const noexcept;

};

}

#include "Classifier.inl"