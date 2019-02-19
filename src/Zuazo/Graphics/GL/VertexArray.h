#pragma once

#include <glad/glad.h>
#include <vector>
#include <array>

#include "UniqueBinding.h"
#include "Buffers/VertexBuffer.h"
#include "Types.h"
#include "../../Utils/Vector.h"

namespace Zuazo::Graphics::GL{

template<typename T>
class VertexArray : public Bindable{
public:
	VertexArray();
	VertexArray(const VertexArray& other)=delete;
	VertexArray(VertexArray&& other);
	~VertexArray();

	void				bind() const override;
	void				unbind() const override;

	void				setAttribPtr(const Buffers::VertexBuffer<T>& buf, u_int32_t no);
	void				enableAttrib(u_int32_t attrib);
	void				disableAttrib(u_int32_t attrib);
private:
	GLuint				m_vao;
};

/*
 * VERTEX ARRAY METHODS
 */

template<typename T>
inline VertexArray<T>::VertexArray(){
	glGenVertexArrays(1, &m_vao);
}

template<typename T>
inline VertexArray<T>::VertexArray(VertexArray<T>&& other){
	m_vao=other.m_vao;
	other.m_vao=0;
}

template<typename T>
inline VertexArray<T>::~VertexArray(){
	glDeleteVertexArrays(1, &m_vao);
}

template<typename T>
inline void VertexArray<T>::bind() const{
	glBindVertexArray(m_vao);
}

template<typename T>
inline void	VertexArray<T>::unbind() const{
	glBindVertexArray(0);
}

template<typename T>
inline void	VertexArray<T>::setAttribPtr(const Buffers::VertexBuffer<T>& buf, u_int32_t no){
	UniqueBinding<VertexArray<T>> vaoBinding(*this);
	UniqueBinding<Buffers::VertexBuffer<T>> vboBinding(buf);

	glVertexAttribPointer(
			no,							//Index
			buf.getVertDim(),			//Size
			GLType<T>,						//Type
			GL_FALSE,					//Normalize?
			0,							//Stride (0=default)
			nullptr						//Offset
	);
}

template<typename T>
inline void	VertexArray<T>::enableAttrib(u_int32_t attrib){
	UniqueBinding<VertexArray<T>> vaoBinding(*this);

	glEnableVertexAttribArray(attrib);
}

template<typename T>
inline void	VertexArray<T>::disableAttrib(u_int32_t attrib){
	UniqueBinding<VertexArray<T>> vaoBinding(*this);

	glDisableVertexAttribArray(attrib);
}
}

