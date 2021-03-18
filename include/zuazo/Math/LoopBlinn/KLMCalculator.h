#pragma once

#include "Classifier.h"
#include "../Vector.h"

#include <array>

namespace Zuazo::Math::LoopBlinn {

template<typename T>
struct KLMCalculator {
	using value_type = T;
	using curve_type = typename Classifier<T>::curve_type;
	using classification_type = typename Classifier<T>::Result;
	using klm_type = Vec3<value_type>;

	struct Result {
		std::array<klm_type, curve_type::size()>values;	
		value_type								subdivisionParameter = std::numeric_limits<value_type>::quiet_NaN();
		bool									isLineOrPoint = false;
		bool									reverse = false;															
	};

	constexpr Result operator()(const classification_type& c) const noexcept;
};

}

#include "KLMCalculator.inl"