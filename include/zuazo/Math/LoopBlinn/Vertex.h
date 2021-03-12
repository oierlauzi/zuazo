#pragma once

#include "../Vector.h"

namespace Zuazo::Math::LoopBlinn {

template<typename T>
struct Vertex {
	using value_type = T;
	using position_vector_type = Vec2<value_type>;
	using klm_vector_type = Vec3<value_type>;

	constexpr Vertex(	const position_vector_type& pos = position_vector_type(0),
						const klm_vector_type& klm = klm_vector_type(-1),
						const value_type& dist = value_type(1) ) noexcept;

	position_vector_type	pos;
	klm_vector_type			klm;
	value_type				dist;
};

}

#include "Vertex.inl"