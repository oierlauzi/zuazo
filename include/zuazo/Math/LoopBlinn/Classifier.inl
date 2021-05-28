#include "Classifier.h"

#include "../Approximation.h"
#include "../Geometry.h"

//This code is based on:
//https://opensource.apple.com/source/WebCore/WebCore-1298.39/platform/graphics/gpu/LoopBlinnClassifier.cpp.auto.html
//https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-25-rendering-vector-art-gpu
//https://www.microsoft.com/en-us/research/wp-content/uploads/2005/01/p1000-loop.pdf

namespace Zuazo::Math::LoopBlinn {


template<typename T>
constexpr Classifier<T>::Result::Result(CurveType type,
										value_type d1,
										value_type d2,
										value_type d3,
										value_type discriminantTerm1 ) noexcept
	: type(type)
	, d1(d1)
	, d2(d2)
	, d3(d3)
	, discriminantTerm1(discriminantTerm1)
{
}



template<typename T>
constexpr typename Classifier<T>::Result Classifier<T>::operator()(const curve_type& curve) const noexcept{
	Result result;
	constexpr value_type EPSILON = 5e-4;

	//Calculate the mixed product of the control points. Note that we're using
	//a affine space (1.0f at the end of the vector)
	const auto a1 = mixed( //b0.(b3 x b2)
		Vec3<value_type>(curve[0], 1), 
		Vec3<value_type>(curve[3], 1), 
		Vec3<value_type>(curve[2], 1)
	); 
	const auto a2 = mixed( //b1.(b0 x b3)
		Vec3<value_type>(curve[1], 1), 
		Vec3<value_type>(curve[0], 1), 
		Vec3<value_type>(curve[3], 1)
	); 
	const auto a3 = mixed( //b2.(b1 x b0)
		Vec3<value_type>(curve[2], 1), 
		Vec3<value_type>(curve[1], 1), 
		Vec3<value_type>(curve[0], 1) //Errata in GPU Gems 3
	);

	//In order to avoid numerical inestability, normalize the values.
	//This won't affect the result. Idea from Apple's WebCore impl
	auto D = Vec3<value_type>(
		a1 - 2*a2 + 3*a3,
		-a2 + 3*a3,
		3*a3
	);
	if(length2(D) > EPSILON) {
		//Only normalize if the length is non-zero
		D = normalize(D);
	}

	//Obtain the d values, rounding to zero small values
	result.d1 = approxZero(D.x, EPSILON);
	result.d2 = approxZero(D.y, EPSILON);
	result.d3 = approxZero(D.z, EPSILON);

	if(	!approxZero(distance2(curve[0], curve[1]), EPSILON) && //distance2 is cheaper than length
		!approxZero(distance2(curve[1], curve[2]), EPSILON) &&
		!approxZero(distance2(curve[2], curve[3]), EPSILON) )
	{
		result.type = CurveType::point;
	} else {
		result.discriminantTerm1 = 3*D.y*D.y - 4*D.x*D.z; //3d2^2 - 4d1d3
		const auto discriminant = approxZero(D.x*D.x*result.discriminantTerm1, EPSILON); //d1^2 * above

		if(!discriminant) {
			if(!result.d1 && !result.d2) {
				if(!result.d3) {
					//All zero: line
					result.type = CurveType::line;
				} else {
					//d1 and d2 = 0, d3 != 0: quadratic
					result.type = CurveType::quadratic;
				}
			} else if(!result.d1) {
				//d=1 but term1 != 0: Special case of cusp
				result.type = CurveType::cusp;
			} else {
				//disc=0: Cusp, edge case of serpentine or loop
				//As term1 will not be exactly 0, decide between 
				//loop and serpentine to avoid NaN as a result of
				//sqrt. Source: Apple's WebCore
				if(result.discriminantTerm1 < 0) {
					result.type = CurveType::loop;
				} else {
					result.type = CurveType::serpentine;
				}
			}
		} else if(discriminant < 0) {
			result.type = CurveType::loop;
		} else {
			result.type = CurveType::serpentine;
		}
	}

	assert(result.type != CurveType::unknown); //Ensure it has been written
	return result;
}

}