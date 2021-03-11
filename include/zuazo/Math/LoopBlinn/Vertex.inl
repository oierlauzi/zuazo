#include "Vertex.h"

namespace Zuazo::Math::LoopBlinn {

template<typename T>
constexpr Vertex<T>::Vertex(const position_vector_type& pos,
							const klm_vector_type& klm ) noexcept
	: pos(pos)
	, klm(klm)
{
}

}