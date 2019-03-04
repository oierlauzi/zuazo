#pragma once

#include <glad/glad.h>

#include "../GL/UniqueBinding.h"
#include "../GL/VertexArray.h"
#include "Shape.h"

namespace Zuazo::Graphics::Shapes{

template <int dim>
class Quads : public ShapeBase<dim, 4>{
public:
	using ShapeBase<dim, 4>::ShapeBase;
	using ShapeBase<dim, 4>::uploadVertices;
	using ShapeBase<dim, 4>::uploadTexCoords;

	void draw() const override{
		GL::UniqueBinding<GL::VertexArray> vaoBinding(ShapeBase<dim, 4>::m_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void drawOutline() const override{
		GL::UniqueBinding<GL::VertexArray> vaoBinding(ShapeBase<dim, 4>::m_vao);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
	}
};
}
