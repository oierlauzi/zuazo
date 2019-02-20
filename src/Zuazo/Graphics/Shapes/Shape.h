#pragma once

#include <array>

#include "../../Utils/Vector.h"
#include "../GL/Buffers/VertexBuffer.h"
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
	using Vertices = std::array<Utils::Vec<dim, float>, nVert>;
	using TexCoords = std::array<Utils::Vec<2, float>, nVert>;

	ShapeBase();
	ShapeBase(const Vertices& vert);
	ShapeBase(const Vertices& vert, const TexCoords& texCoord);
	ShapeBase(const ShapeBase& other)=delete; //TODO
	ShapeBase(ShapeBase&& other)=default;
	virtual ~ShapeBase()=default;
protected:
	GL::VertexArray<float>				m_vao;

	void								uploadVertices(const Vertices& vertices);
	void								uploadTexCoords(const TexCoords& texCoords);
private:
	GL::Buffers::VertexBuffer<float>	m_vertexVbo;
	GL::Buffers::VertexBuffer<float>	m_texCoordVbo;

	Vertices							m_vertexData;
	TexCoords							m_texCoordData;
};

template <int dim, int nVert>
inline ShapeBase<dim, nVert>::ShapeBase(){
	m_vao.enableAttrib(0);
	m_vao.setAttribPtr(m_vertexVbo, 0);
	m_vao.enableAttrib(1);
	m_vao.setAttribPtr(m_texCoordData, 1);
}

template <int dim, int nVert>
inline ShapeBase<dim, nVert>::ShapeBase(const Vertices& vert) :
	m_vertexVbo(vert),
	m_vertexData(vert)
{
	m_vao.enableAttrib(0);
	m_vao.setAttribPtr(m_vertexVbo, 0);
	m_vao.enableAttrib(1);
	m_vao.setAttribPtr(m_texCoordData, 1);
}

template <int dim, int nVert>
inline ShapeBase<dim, nVert>::ShapeBase(const Vertices& vert, const TexCoords& texCoord) :
	m_vertexVbo(vert),
	m_texCoordVbo(texCoord),
	m_vertexData(vert),
	m_texCoordData(texCoord)
{
	m_vao.enableAttrib(0);
	m_vao.setAttribPtr(m_vertexVbo, 0);
	m_vao.enableAttrib(1);
	m_vao.setAttribPtr(m_texCoordData, 1);
}

template <int dim, int nVert>
inline void	ShapeBase<dim, nVert>::uploadVertices(const Vertices& vertices){
	if(memcmp(&(m_vertexData[0]), &(vertices[0]), sizeof(float) * dim * nVert)){
		//Vertices have changed. Upload changes
		m_vertexVbo.upload(vertices);
		m_vao.setAttribPtr(m_vertexVbo, 0);
		memcpy(&(m_vertexData[0]), &(vertices[0]), sizeof(float) * dim * nVert);
	}
}

template <int dim, int nVert>
inline void	ShapeBase<dim, nVert>::uploadTexCoords(const TexCoords& texCoords){
	if(memcmp(&(m_vertexData[0]), &(texCoords[0]), sizeof(float) * 2 * nVert)){
		//Texture coordinates have changed. Upload changes
		m_texCoordVbo.upload(texCoords);
		m_vao.setAttribPtr(m_texCoordVbo, 1); //TODO necesary?
		memcpy(&(m_texCoordData[0]), &(texCoords[0]), sizeof(float) * 2 * nVert);
	}
}

}
