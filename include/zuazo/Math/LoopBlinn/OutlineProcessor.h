#pragma once

#include "../Vector.h"
#include "../BezierLoop.h"
#include "../Triangulator.h"
#include "../../Utils/BufferView.h"

#include <utility>

namespace Zuazo::Math::LoopBlinn {

template<typename T, typename I = uint32_t>
class OutlineProcessor {
public:
	using value_type = T;
	using index_type = I;
	using vector_type = Vec2<value_type>;
	using bezier_type = CubicBezier<vector_type>;
	using contour_type = CubicBezierLoop<vector_type>;
	using polygon_type = Polygon<value_type>;

	struct Vertex {
		constexpr Vertex(	Vec2<value_type> pos = Vec2<value_type>(0), 
							Vec3<value_type> klm = Vec3<value_type>(-1, 0, 0) )
			: pos(pos)
			, klm(klm)
		{
		}

		Vec2<value_type> 	pos;
		Vec3<value_type> 	klm;
	};

	OutlineProcessor(index_type primitiveRestartIndex = ~index_type(0));
	OutlineProcessor(const OutlineProcessor& other) = delete;
	OutlineProcessor(OutlineProcessor&& other) = default;
	~OutlineProcessor() = default;

	OutlineProcessor&						operator=(const OutlineProcessor& other) = delete;
	OutlineProcessor&						operator=(OutlineProcessor&& other) = default;

	void									clear();

	void									addBezier(const bezier_type& bezier);
	void									addPolygon(const polygon_type& polygon);
	void									addContour(const contour_type& contour);
	void									addOutline(Utils::BufferView<const contour_type> outline);

	Utils::BufferView<const Vertex>			getVertices() const noexcept;
	Utils::BufferView<const index_type>		getIndices() const noexcept;

	
private:
	index_type 								m_primitiveRestartIndex;

	std::vector<Vertex>						m_vertices;
	std::vector<index_type>					m_indices;

	Polygon<value_type>						m_innerHull;
	Triangulator<value_type, index_type>	m_triangulator;

	
};

}

#include "OutlineProcessor.inl"