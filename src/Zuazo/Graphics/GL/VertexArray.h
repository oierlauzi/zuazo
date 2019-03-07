#pragma once

#include <glad/glad.h>

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
}

