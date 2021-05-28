#pragma once

#include "../Bezier.h"

#include <limits>

namespace Zuazo::Math::LoopBlinn {

enum class CurveType {
	unknown = -1,
	point,
	line,
	quadratic,
	serpentine,
	cusp,
	loop,
};

template<typename T>
struct Classifier {
	using value_type = T;
	using curve_type = CubicBezier<Vec2<value_type>>;

	struct Result {
		constexpr Result(	CurveType type = CurveType::unknown,
							value_type d1 = std::numeric_limits<value_type>::quiet_NaN(),
							value_type d2 = std::numeric_limits<value_type>::quiet_NaN(),
							value_type d3 = std::numeric_limits<value_type>::quiet_NaN(),
							value_type discriminantTerm1 = std::numeric_limits<value_type>::quiet_NaN() ) noexcept;

		CurveType			type;
		value_type			d1;
		value_type			d2;
		value_type			d3;
		value_type			discriminantTerm1 ;
	};

	constexpr Result operator()(const curve_type& curve) const noexcept;

};

}

#include "Classifier.inl"
