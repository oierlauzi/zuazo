#include "KLMCalculator.h"

#include "../Matrix.h"

//This code is based on:
//https://opensource.apple.com/source/WebCore/WebCore-1298.39/platform/graphics/gpu/LoopBlinnClassifier.cpp.auto.html
//https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-25-rendering-vector-art-gpu
//https://www.microsoft.com/en-us/research/wp-content/uploads/2005/01/p1000-loop.pdf

namespace Zuazo::Math::LoopBlinn {

template<typename T>
constexpr KLMCalculator<T>::Result::Result(	const std::array<klm_type, curve_type::size()>& values,
											value_type subdivisionParameter,
											DegeneratedCurveType degeneratedType,
											bool reverse ) noexcept
	: values(values)
	, subdivisionParameter(subdivisionParameter)
	, degeneratedType(degeneratedType)
	, reverse(reverse)
{
}



template<typename T>
constexpr typename KLMCalculator<T>::Result KLMCalculator<T>::operator()(const classification_type& c) const noexcept 
{
	Result result;

	switch (c.type) {
	case CurveType::point: {
		result.degeneratedType = DegeneratedCurveType::point;

		break;
	}
	case CurveType::line: {
		result.degeneratedType = DegeneratedCurveType::line;

		constexpr auto NOT_USED = std::numeric_limits<value_type>::quiet_NaN();
		result.values = {
			Vec3<value_type>(1),
			Vec3<value_type>(NOT_USED), //Should not be used
			Vec3<value_type>(NOT_USED), //Should not be used
			Vec3<value_type>(1)
		};

		result.reverse = false;

		break;
	}
	case CurveType::quadratic: {
		result.degeneratedType = DegeneratedCurveType::quadratic;

		constexpr auto THIRD = value_type(1) / value_type(3);
		result.values = {
			Vec3<value_type>(0, 		0, 			0		),
			Vec3<value_type>(THIRD, 	0, 			THIRD	),
			Vec3<value_type>(2*THIRD, 	THIRD, 		2*THIRD	),
			Vec3<value_type>(1, 		1, 			1		)
		};

		result.reverse = c.d3 < 0;

		break;
	}
	case CurveType::serpentine: {
		result.degeneratedType = DegeneratedCurveType::cubic;

		assert(c.discriminantTerm1 >= 0); //To avoid NaNs
		const auto sqrt3DiscTerm1 = sqrt(3*c.discriminantTerm1);
		const auto ls = 3*c.d2 - sqrt3DiscTerm1;
		const auto ms = 3*c.d2 + sqrt3DiscTerm1;
		const auto lt = 6*c.d1;
		const auto mt = lt;

		const auto Lsmt = ls - lt;
		const auto Msmt = ms - mt;

		result.values = {
			Vec3<value_type>(
				ls*ms,											//ls*ms, 			
				ls*ls*ls,										//ls^3
				ms*ms*ms										//ms^3
			), 
			Vec3<value_type>(
				(3*ls*ms - ls*mt - lt*ms)/3, 					//ls*ms - 1/3*ls*mt - 1/3*lt*ms
				ls*ls*Lsmt,										//ls^2*(ls-lt) 
				ms*ms*Msmt										//ms^2*(ms-mt)
			), 			
			Vec3<value_type>(
				(3*ls*ms - 2*ls*mt - 2*lt*ms + lt*mt)/3,		//ls*ms - 2/3*ls*mt - 2/3*lt*ms + 1/3*lt*mt
				ls*Lsmt*Lsmt, 									//ls*(ls-lt)^2
				ms*Msmt*Msmt									//ms*(ms-mt)^2
			), 
			Vec3<value_type>(
				Lsmt*Msmt, 										//(ls-lt)*(ms-mt)
				Lsmt*Lsmt*Lsmt, 								//(ls-lt)^3
				Msmt*Msmt*Msmt									//(ms-mt)^3
			)
		};

		result.reverse = c.d1 < 0;

		break;
	}
	case CurveType::cusp: {
		result.degeneratedType = DegeneratedCurveType::cubic;

		const auto ls = c.d3;
		const auto lt = 3*c.d2;
		const auto Lsmt = ls - lt;

		result.values = {
			Vec3<value_type>(
				ls,												//ls, 
				ls*ls*ls,										//ls^3
				1.0f											//1
			),
			Vec3<value_type>(
				ls - 1*lt/3,									//ls - 1/3*lt
				ls*ls*Lsmt,										//ls^2*(ls-lt)
				1.0f											//1
			), 
			Vec3<value_type>(
				ls - 2*lt/3,									//ls - 2/3*lt
				ls*Lsmt*Lsmt,									//ls*(ls-lt)^2
				1.0f											//1
			),	
			Vec3<value_type>(
				Lsmt,											//ls-lt	
				Lsmt*Lsmt*Lsmt,									//(ls-lt)^3
				1.0f											//1
			) 
		};

		result.reverse = false; //Never reverses

		break;
	}
	case CurveType::loop: {
		result.degeneratedType = DegeneratedCurveType::cubic;

		assert(c.discriminantTerm1 < 0); //To avoid NaNs
		const auto sqrtDisc = sqrt(-c.discriminantTerm1);
		const auto ls = c.d2 - sqrtDisc;
		const auto ms = c.d2 + sqrtDisc;
		const auto lt = 2*c.d1;
		const auto mt = lt;

		//Check if double point:
		//Based on:
		//https://opensource.apple.com/source/WebCore/WebCore-1298.39/platform/graphics/gpu/LoopBlinnTextureCoords.cpp.auto.html
		const float ql = ls / lt;
		const float qm = ms / mt;

		//Check if subdivision will be required
		if(isInRangeExclusive(ql, value_type(0), value_type(1))) {
			result.subdivisionParameter = ql;
		} else if(isInRangeExclusive(qm, value_type(0), value_type(1))) {
			result.subdivisionParameter = qm;
		} else {
			const auto Lsmt = ls - lt;
			const auto Msmt = ms - mt;

			result.values = {
				Vec3<value_type>(
					ls*ms,										//ls*ms
					ls*ls*ms,									//ls^2*ms
					ls*ms*ms									//ls*ms^2
				),		 			
				Vec3<value_type>(
					(3*ls*ms - ls*mt - lt*ms)/3,				//ls*ms - 1/3*ls*mt - 1/3*lt*ms. Errata in GPU Gems 3
					(3*ls*ls*ms - 2*ls*lt*ms - ls*ls*mt)/3,		//ls^2*ms - 2/3*ls*lt*ms - 1/3*ls^2*mt
					(3*ls*ms*ms - 2*ls*ms*mt - lt*ms*ms)/3		//ls*ms^2 - 2/3*ls*ms*mt - 1/3*lt*ms^2
				), 
				Vec3<value_type>(
					(3*ls*ms - 2*ls*mt - 2*lt*ms + lt*mt)/3,	//ls*ms - 2/3*ls*mt - 2/3*lt*ms + 1/3*lt*mt
					Lsmt*(3*ls*ms - 2*ls*mt - lt*ms)/3, 		//(ls-lt)*(ls*ms - 2/3*ls*mt - 1/3*lt*ms)
					Msmt*(3*ls*ms - 2*lt*ms - ls*mt)/3 			//(ms-mt)*(ls*ms - 2/3*lt*ms - 1/3*ls*mt)
				),
				Vec3<value_type>(
					Lsmt*Msmt,									//(ls-lt)*(ms-mt) 
					Lsmt*Lsmt*Msmt,								//(ls-lt)^2*(ms-mt) 
					Lsmt*Msmt*Msmt								//(ls-lt)*(ms-mt)^2
				)
			};
			
			result.reverse = sign(result.values.front().x) != sign(c.d1);
			
		}

		break;
	}
	default:
		assert(false);
		break;
	}

	assert(result.degeneratedType != DegeneratedCurveType::unknown);
	return result;
}

}