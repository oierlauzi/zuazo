#pragma once

#include <array>

#include "../../Utils/Vector.h"
#include "../GL/Buffer.h"
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
	m_texCoordVbo.allocate(s_texCoordArraySize, GL::VertexArrayBuffer::BufferUsage::StreamDraw);
	m_vertexVbo.allocate(s_vertexArraySize, GL::VertexArrayBuffer::BufferUsage::StreamDraw);

	m_vao.enableAttrib(0);
	m_vao.enableAttrib(1);
	m_vao.setAttribPtr<VectorComponent, dim, 0>(m_vertexVbo);
	m_vao.setAttribPtr<VectorComponent, dim, 1>(m_texCoordVbo);
}

template <int dim, int nVert>
inline ShapeBase<dim, nVert>::ShapeBase(const Vertices& vert) :
	m_vertexVbo(vert.data(), s_vertexArraySize),
	m_vertexData(vert)
{
	m_texCoordVbo.allocate(s_texCoordArraySize, GL::VertexArrayBuffer::BufferUsage::StreamDraw);

	m_vao.enableAttrib(0);
	m_vao.enableAttrib(1);
	m_vao.setAttribPtr<VectorComponent, dim, 0>(m_vertexVbo);
	m_vao.setAttribPtr<VectorComponent, dim, 1>(m_texCoordVbo);
}

template <int dim, int nVert>
inline ShapeBase<dim, nVert>::ShapeBase(const Vertices& vert, const TexCoords& texCoord) :
	m_vertexVbo(vert.data(), s_vertexArraySize),
	m_texCoordVbo(texCoord.data(), s_texCoordArraySize),
	m_vertexData(vert),
	m_texCoordData(texCoord)
{
	m_vao.enableAttrib(0);
	m_vao.enableAttrib(1);
	m_vao.setAttribPtr<VectorComponent, dim, 0>(m_vertexVbo);
	m_vao.setAttribPtr<VectorComponent, dim, 1>(m_texCoordVbo);
}

template <int dim, int nVert>
inline void	ShapeBase<dim, nVert>::uploadVertices(const Vertices& vertices){
	if(memcmp(m_vertexData.data(), vertices.data(), s_vertexArraySize)){
		//Vertices have changed. Upload changes
		m_vertexVbo.write(vertices.data());
		m_vao.setAttribPtr<VectorComponent, dim, 0>(m_vertexVbo); //TODO necesary?
		memcpy(m_vertexData.data(), vertices.data(), s_vertexArraySize);
	}
}

template <int dim, int nVert>
inline void	ShapeBase<dim, nVert>::uploadTexCoords(const TexCoords& texCoords){
	if(memcmp(m_texCoordData.data(), texCoords.data(), s_texCoordArraySize)){
		//Texture coordinates have changed. Upload changes
		m_texCoordVbo.write(texCoords.data());
		m_vao.setAttribPtr<VectorComponent, dim, 1>(m_texCoordVbo); //TODO necesary?
		memcpy(m_texCoordData.data(), texCoords.data(), s_texCoordArraySize);
	}
}

}
