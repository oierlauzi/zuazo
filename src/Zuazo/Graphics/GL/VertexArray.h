#pragma once

#include <GL/glew.h>
#include <vector>
#include <array>

#include "UniqueBinding.h"
#include "Types.h"
#include "../../Utils/Vector.h"
/*
namespace Zuazo::Graphics::GL{

template<typename T, u_int32_t att>
class VertexArray : public Bindable{
public:
	VertexArray();
	VertexArray(const std::array<VertexBuffer<T>, att>& data);
	VertexArray(const VertexArray& other)=delete;
	VertexArray(VertexArray&& other);
	~VertexArray();

	void				bind() const override;
	void				unbind() const override;

	template<u_int32_t no>
	void				setAttribPtr(const VertexBuffer<T>& buf);
	void				setAttribPtr(const VertexBuffer<T>& buf, u_int32_t no);
	void				setAttribPtr(const std::array<VertexBuffer<T>, att>& data);
private:
	GLuint				m_vao;
};
*/
/*
 * VERTEX ARRAY METHODS
 */
/*
template<typename T, u_int32_t att>
inline VertexArray<T, att>::VertexArray(){
	glGenVertexArrays(1, &m_vao);

	for(u_int32_t i=0; i<att; i++){
		glEnableVertexAttribArray(i);
	}
}

template<typename T, u_int32_t att>
inline VertexArray<T, att>::VertexArray(const std::array<VertexBuffer<T>, att>& data) : VertexArray(){
	setAttribPtr(data);
}

template<typename T, u_int32_t att>
inline VertexArray<T, att>::VertexArray(VertexArray<T, att>&& other){
	m_vao=other.m_vao;
	other.m_vao=0;
}

template<typename T, u_int32_t att>
inline VertexArray<T, att>::~VertexArray(){
	glDeleteVertexArrays(1, &m_vao);
}

template<typename T, u_int32_t att>
inline void VertexArray<T, att>::bind() const{
	glBindVertexArray(m_vao);
}

template<typename T, u_int32_t att>
inline void	VertexArray<T, att>::unbind() const{
	glBindVertexArray(0);
}

template<typename T, u_int32_t att>
template<u_int32_t no>
inline void	VertexArray<T, att>::setAttribPtr(const VertexBuffer<T>& buf){
	UniqueBinding<VertexArray<T, att>> vao(*this);
	UniqueBinding<VertexBuffer<T>> vbo(buf);

	glVertexAttribPointer(
			no,							//Index
			buf.getVertDim(),			//Size
			GL<T>,						//Type
			GL_FALSE,					//Normalize?
			0,							//Stride (0=default)
			nullptr						//Offset
	);
}

template<typename T, u_int32_t att>
inline void	VertexArray<T, att>::setAttribPtr(const VertexBuffer<T>& buf, u_int32_t no){
	UniqueBinding<VertexArray<T, att>> vao(*this);
	UniqueBinding<VertexBuffer<T>> vbo(buf);

	glVertexAttribPointer(
			no,							//Index
			buf.getVertDim(),			//Size
			GL<T>,						//Type
			GL_FALSE,					//Normalize?
			0,							//Stride (0=default)
			nullptr						//Offset
	);
}

template<typename T, u_int32_t att>
inline void	VertexArray<T, att>::setAttribPtr(const std::array<VertexBuffer<T>, att>& data){
	UniqueBinding<VertexArray<T, att>> vao(*this);

	for(u_int32_t i=0; i<att; i++){
		UniqueBinding<VertexBuffer<T>> vbo(data[i]);

		glVertexAttribPointer(
				i,							//Index
				data[i].getVertDim(),		//Size
				GL<T>,						//Type
				GL_FALSE,					//Normalize?
				0,							//Stride (0=default)
				nullptr						//Offset
		);
	}
}





}
*/
