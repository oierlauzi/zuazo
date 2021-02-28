#pragma once

#include "Classifier.h"
#include "KLMCalculator.h"
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
	using position_vector_type = Vec2<value_type>;
	using klm_vector_type = Vec3<value_type>;
	using bezier_type = CubicBezier<position_vector_type>;
	using contour_type = CubicBezierLoop<position_vector_type>;
	using polygon_type = Polygon<value_type>;

	static constexpr klm_vector_type KLM_EDGE = klm_vector_type(0);
	static constexpr klm_vector_type KLM_FILL = klm_vector_type(-1);

	struct Vertex {
		constexpr Vertex(	position_vector_type pos = position_vector_type(0), 
							klm_vector_type klm = KLM_FILL )
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

	void									addBezier(	const bezier_type& bezier, 
														FillSide fillSide = FillSide::LEFT);
	void									addPolygon(const polygon_type& polygon);
	void									addContour(const contour_type& contour);
	void									addOutline(Utils::BufferView<const contour_type> outline);

	Utils::BufferView<const Vertex>			getVertices() const noexcept;
	Utils::BufferView<const index_type>		getIndices() const noexcept;

	
private:
	index_type 								m_primitiveRestartIndex;

	std::vector<Vertex>						m_vertices;
	std::vector<index_type>					m_indices;

	contour_type							m_ccwContour;
	polygon_type							m_innerHull;
	Triangulator<value_type, index_type>	m_triangulator;

	
};

}

#include "OutlineProcessor.inl"