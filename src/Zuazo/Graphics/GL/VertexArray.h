#pragma once

#include <glad/glad.h>
#include <vector>
#include <array>

#include "UniqueBinding.h"
#include "Buffer.h"
#include "Types.h"
#include "../../Utils/Vector.h"

namespace Zuazo::Graphics::GL{

class VertexArray{
public:
	VertexArray();
	VertexArray(const VertexArray& other)=delete;
	VertexArray(VertexArray&& other);
	~VertexArray();

	void				bind() const;
	void				unbind() const;

	template<typename T, u_int32_t dim, u_int32_t no>
	void				setAttribPtr(const Buffer<BufferTypes::Array>& buf);
	void				enableAttrib(u_int32_t attrib);
	void				disableAttrib(u_int32_t attrib);
private:
	GLuint				m_vao;
};

/*
 * VERTEX ARRAY METHODS
 */


inline VertexArray::VertexArray(){
	glGenVertexArrays(1, &m_vao);
}


inline VertexArray::VertexArray(VertexArray&& other){
	m_vao=other.m_vao;
	other.m_vao=0;
}


inline VertexArray::~VertexArray(){
	glDeleteVertexArrays(1, &m_vao);
}


inline void VertexArray::bind() const{
	glBindVertexArray(m_vao);
}


inline void	VertexArray::unbind() const{
	glBindVertexArray(0);
}

template<typename T, u_int32_t dim, u_int32_t no>
inline void	VertexArray::setAttribPtr(const VertexArrayBuffer& buf){
	UniqueBinding<VertexArray> vaoBinding(*this);
	UniqueBinding<VertexArrayBuffer> vboBinding(buf);

	glVertexAttribPointer(
			no,							//Index
			dim,						//Size
			GLType<T>,					//Type
			GL_FALSE,					//Normalize?
			0,							//Stride (0=default)
			nullptr						//Offset
	);
}


inline void	VertexArray::enableAttrib(u_int32_t attrib){
	UniqueBinding<VertexArray> vaoBinding(*this);

	glEnableVertexAttribArray(attrib);
}


inline void	VertexArray::disableAttrib(u_int32_t attrib){
	UniqueBinding<VertexArray> vaoBinding(*this);

	glDisableVertexAttribArray(attrib);
}
}

