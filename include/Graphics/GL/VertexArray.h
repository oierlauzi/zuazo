#pragma once

#include "../../glad/glad.h"
#include "Types.h"

#include <sys/types.h>

namespace Zuazo::Graphics::GL{

class VertexArray{
public:
	VertexArray();
	VertexArray(const VertexArray& other)=delete;
	VertexArray(VertexArray&& other);
	~VertexArray();

	void				bind() const;
	void				unbind() const;

	GLuint				get() const;

	static void			enableAttribute(u_int32_t no);
	static void			disableAttribute(u_int32_t no);
	template <typename T, int dim>
	static void			attributePtr(u_int32_t no);
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

inline GLuint VertexArray::get() const{
	return m_vao;
}

inline void	VertexArray::enableAttribute(u_int32_t no){
	glEnableVertexAttribArray(no);
}

inline void	VertexArray::disableAttribute(u_int32_t no){
	glDisableVertexAttribArray(no);
}

template <typename T, int dim>
inline void	VertexArray::attributePtr(u_int32_t no){
	glVertexAttribPointer(
			no,				//Attribute index
			dim,			//Vertex dimensions
			GLType<T>,		//Vertex component type
			0,				//Stride (0=auto, packed)
			GL_FALSE,		//Normalize?
			0				//Offset
	);
}


}

