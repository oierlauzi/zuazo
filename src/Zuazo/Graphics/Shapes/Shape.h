#pragma once

#include <glad/glad.h>
#include <stddef.h>
#include <array>

#include "../../Utils/Vector.h"
#include "../GL/Buffer.h"
#include "../GL/Types.h"
#include "../GL/UniqueBinding.h"
#include "../GL/VertexArray.h"

namespace Zuazo::Graphics::Shapes{

class Shape{
public:
	virtual ~Shape()=default;

	virtual void 										draw() const=0;
	virtual void 										drawOutline() const=0;
};

template <int dim, int nVert>
class ShapeBase : public Shape{
public:
	typedef float VectorComponent;
	typedef std::array<Utils::Vec<dim,	VectorComponent>, nVert> Vertices;
	typedef std::array<Utils::Vec<2, 	VectorComponent>, nVert> TexCoords;

	ShapeBase();
	ShapeBase(const Vertices& vert);
	ShapeBase(const Vertices& vert, const TexCoords& texCoord);
	ShapeBase(const ShapeBase& other)=delete; //TODO
	ShapeBase(ShapeBase&& other)=default;
	virtual ~ShapeBase()=default;
protected:
	GL::VertexArray				m_vao;

	void						uploadVertices(const Vertices& vertices);
	void						uploadTexCoords(const TexCoords& texCoords);
private:
	GL::VertexArrayBuffer		m_vertexVbo;
	GL::VertexArrayBuffer		m_texCoordVbo;

	Vertices					m_vertexData;
	TexCoords					m_texCoordData;

	static constexpr size_t		s_vertexArraySize=sizeof(Utils::Vec<dim,	VectorComponent>) * nVert;
	static constexpr size_t		s_texCoordArraySize=sizeof(Utils::Vec<2,	VectorComponent>) * nVert;
};

template <int dim, int nVert>
inline ShapeBase<dim, nVert>::ShapeBase(){
	GL::UniqueBinding vaoBinding(m_vao);

	//Enable arrays
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	{
		GL::UniqueBinding bufferBinding(m_vertexVbo);
		glBufferData(
				m_vertexVbo.GLType,
				s_vertexArraySize,
				nullptr,
				GL_STREAM_DRAW
		);
		glVertexAttribPointer(
				0,
				dim,
				GL::GLType<VectorComponent>,
				0,
				GL_FALSE,
				0
		);
	}

	{
		GL::UniqueBinding bufferBinding(m_texCoordData);
		glBufferData(
				m_texCoordData.GLType,
				s_texCoordArraySize,
				nullptr,
				GL_STREAM_DRAW
		);
		glVertexAttribPointer(
				1,
				2,
				GL::GLType<VectorComponent>,
				0,
				GL_FALSE,
				0
		);
	}
}

template <int dim, int nVert>
inline ShapeBase<dim, nVert>::ShapeBase(const Vertices& vert){
	GL::UniqueBinding vaoBinding(m_vao);

	//Enable arrays
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	m_vertexData=vert;

	{
		GL::UniqueBinding bufferBinding(m_vertexVbo);
		glBufferData(
				m_vertexVbo.GLType,
				s_vertexArraySize,
				m_vertexData.data(),
				GL_STATIC_DRAW
		);
		glVertexAttribPointer(
				0,
				dim,
				GL::GLType<VectorComponent>,
				0,
				GL_FALSE,
				0
		);
	}

	{
		GL::UniqueBinding bufferBinding(m_texCoordVbo);
		glBufferData(
				m_texCoordVbo.GLType,
				s_texCoordArraySize,
				nullptr,
				GL_STREAM_DRAW
		);
		glVertexAttribPointer(
				1,
				2,
				GL::GLType<VectorComponent>,
				0,
				GL_FALSE,
				0
		);
	}
}

template <int dim, int nVert>
inline ShapeBase<dim, nVert>::ShapeBase(const Vertices& vert, const TexCoords& texCoord){
	GL::UniqueBinding vaoBinding(m_vao);

	//Enable arrays
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	m_vertexData=vert;
	m_texCoordData=texCoord;

	{
		GL::UniqueBinding bufferBinding(m_vertexVbo);
		glBufferData(
				m_vertexVbo.GLType,
				s_vertexArraySize,
				m_vertexData.data(),
				GL_STATIC_DRAW
		);
		glVertexAttribPointer(
				0,
				dim,
				GL::GLType<VectorComponent>,
				0,
				GL_FALSE,
				0
		);
	}

	{
		GL::UniqueBinding bufferBinding(m_texCoordVbo);
		glBufferData(
				m_texCoordVbo.GLType,
				s_texCoordArraySize,
				m_vertexData.data(),
				GL_STATIC_DRAW
		);
		glVertexAttribPointer(
				1,
				2,
				GL::GLType<VectorComponent>,
				0,
				GL_FALSE,
				0
		);
	}
}

template <int dim, int nVert>
inline void	ShapeBase<dim, nVert>::uploadVertices(const Vertices& vertices){
	if(memcmp(m_vertexData.data(), vertices.data(), s_vertexArraySize)){
		//Vertices have changed. Upload changes
		memcpy(m_vertexData.data(), vertices.data(), s_vertexArraySize);
		GL::UniqueBinding vaoBinding(m_vao);
		GL::UniqueBinding bufferBinding(m_vertexVbo);

		glBufferSubData(
				m_vertexVbo.GLType,
				0,
				s_vertexArraySize,
				m_vertexData.data()
		);
		glVertexAttribPointer(
				0,
				dim,
				GL::GLType<VectorComponent>,
				0,
				GL_FALSE,
				0
		);
	}
}

template <int dim, int nVert>
inline void	ShapeBase<dim, nVert>::uploadTexCoords(const TexCoords& texCoords){
	if(memcmp(m_texCoordData.data(), texCoords.data(), s_texCoordArraySize)){
		//Texture coordinates have changed. Upload changes
		memcpy(m_texCoordData.data(), texCoords.data(), s_texCoordArraySize);
		GL::UniqueBinding vaoBinding(m_vao);
		GL::UniqueBinding bufferBinding(m_texCoordVbo);

		glBufferSubData(
				m_texCoordVbo.GLType,
				0,
				s_texCoordArraySize,
				m_texCoordData.data()
		);
		glVertexAttribPointer(
				1,
				2,
				GL::GLType<VectorComponent>,
				0,
				GL_FALSE,
				0
		);
	}
}

}
