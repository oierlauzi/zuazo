#pragma once

#include "Context.h"
#include "GL/Shader.h"
#include "GL/Buffer.h"
#include "GL/UniqueBinding.h"
#include "../Utils/Vector.h"
#include "../glad/glad.h"

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <array>

namespace Zuazo::Graphics{


class VertexArray{
public:
	VertexArray()=default;
	VertexArray(const VertexArray& other)=delete;
	VertexArray(VertexArray&& other)=default;
	virtual ~VertexArray()=default;

	template<typename T, int dim>
	void					enableAttribute(GLuint attribute, u_int32_t vertices);
	template<typename T, int dim>
	void					enableAttribute(GLuint attribute, u_int32_t vertices, const Utils::Vec<T, dim>* data);
	template<typename T, int dim>
	void					enableAttribute(GLuint attribute, const std::vector<Utils::Vec<T, dim>>& vertices);
	template<typename T, int dim, size_t nVertices>
	void					enableAttribute(GLuint attribute, const std::array<Utils::Vec<T, dim>, nVertices>& vertices);
	void					disableAttribute(GLuint attribute);
	void					disbleAllAttributes();

	template<typename T, int dim>
	void					uploadVertices(GLuint attribute, const Utils::Vec<T, dim>* data);
	template<typename T, int dim>
	void					uploadVertices(GLuint attribute, const std::vector<Utils::Vec<T, dim>>& vertices);
	template<typename T, int dim, size_t nVertices>
	void					uploadVertices(GLuint attribute, const std::array<Utils::Vec<T, dim>, nVertices>& vertices);

	void					bind() const;
	void					unbind() const;
private:
	struct VBO{
		GL::VertexArrayBuffer 	vbo;
		size_t					size;
	};

	GL::VertexArray			m_vao;
	std::map<GLuint, VBO> 	m_vbos;
};


template<typename T, int dim>
inline void VertexArray::enableAttribute(GLuint attribute, u_int32_t nVertices){
	if(m_vbos.find(attribute) == m_vbos.end()){
		UniqueContext ctx(Context::getMainCtx());
		m_vbos.emplace(attribute);
	}

	GL::UniqueBinding<GL::VertexArrayBuffer> vboBind(m_vbos[attribute].vbo);
	GL::VertexArrayBuffer::bufferData(
			sizeof(T) * dim * nVertices,
			GL::VertexArrayBuffer::Usage::DynamicDraw,
			nullptr
	);

	GL::UniqueBinding<GL::VertexArray> vaoBind(m_vao);
	GL::VertexArray::enableAttribute(attribute);
	GL::VertexArray::attributePtr<T, dim>(attribute);
}

template<typename T, int dim>
inline void VertexArray::enableAttribute(GLuint attribute, u_int32_t nVertices, const Utils::Vec<T, dim>* data){
	if(m_vbos.find(attribute) == m_vbos.end()){
		UniqueContext ctx(Context::getMainCtx());
		m_vbos.emplace(attribute);
	}

	GL::UniqueBinding<GL::VertexArrayBuffer> vboBind(m_vbos[attribute].vbo);
	GL::VertexArrayBuffer::bufferData(
			sizeof(T) * dim * nVertices,
			GL::VertexArrayBuffer::Usage::StaticDraw,
			data
	);

	GL::UniqueBinding<GL::VertexArray> vaoBind(m_vao);
	GL::VertexArray::enableAttribute(attribute);
	GL::VertexArray::attributePtr<T, dim>(attribute);
}

template<typename T, int dim>
inline void VertexArray::enableAttribute(GLuint attribute, const std::vector<Utils::Vec<T, dim>>& vertices){
	enableAttribute(attribute, vertices.size(), vertices.data());
}

template<typename T, int dim, size_t nVertices>
inline void VertexArray::enableAttribute(GLuint attribute, const std::array<Utils::Vec<T, dim>, nVertices>& vertices){
	enableAttribute(attribute, nVertices, vertices.data());
}

inline void VertexArray::disableAttribute(GLuint attribute){
	GL::UniqueBinding<GL::VertexArray> vaoBind(m_vao);
	GL::VertexArray::disableAttribute(attribute);
	m_vbos.erase(attribute);
}

inline void VertexArray::disbleAllAttributes(){
	auto ite=m_vbos.begin();
	GL::UniqueBinding<GL::VertexArray> vaoBind(m_vao);
	while(ite != m_vbos.end()){
		GL::VertexArray::disableAttribute(ite->first);
		ite=m_vbos.erase(ite);
	}
}

template<typename T, int dim>
inline void VertexArray::uploadVertices(GLuint attribute, const Utils::Vec<T, dim>* data){
	auto vboIte=m_vbos.find(attribute);
	if(vboIte != m_vbos.end()){
		GL::UniqueBinding<GL::VertexArrayBuffer> vboBind(vboIte->second.vbo);
		GL::VertexArrayBuffer::bufferSubData(
				vboIte->second.size,
				0,
				data
		);
	}
}

template<typename T, int dim>
inline void VertexArray::uploadVertices(GLuint attribute, const std::vector<Utils::Vec<T, dim>>& vertices){
	uploadVertices(attribute, vertices.data());
}

template<typename T, int dim, size_t nVertices>
inline void VertexArray::uploadVertices(GLuint attribute, const std::array<Utils::Vec<T, dim>, nVertices>& vertices){
	uploadVertices(attribute, vertices.data());
}


inline void VertexArray::bind() const{
	m_vao.bind();
}

inline void VertexArray::unbind() const{
	m_vao.unbind();
}

}
