#pragma once

#include <GL/glew.h>
#include <vector>
#include <array>

#include "UniqueBinding.h"
#include "Types.h"
#include "../../Utils/Vector.h"

namespace Zuazo::Graphics::GL{

template<typename T>
class VertexBuffer : public Bindable{
public:
	VertexBuffer();

	template<int dim>
	VertexBuffer(const Utils::Vec<dim, T>* vert, u_int32_t nVert);
	template<int dim>
	VertexBuffer(const std::vector<Utils::Vec<dim, T>>& vert);
	VertexBuffer(const VertexBuffer<T>& other);
	VertexBuffer(VertexBuffer<T>&& other);
	~VertexBuffer();

	void				bind() const override;
	void				unbind() const override;

	template<int dim>
	void				upload(const Utils::Vec<dim, T>* vert, u_int32_t nVert);
	template<int dim>
	void				upload(const std::vector<Utils::Vec<dim, T>>& vert);

	size_t				getSize() const;
	u_int32_t			getVertNo() const;
	u_int32_t			getVertDim() const;
private:
	GLuint				m_vbo;

	u_int32_t			m_nVert;
	u_int32_t			m_nDim;
	size_t				m_size;

	T*					m_data;

	void				updateData();
};

template<typename T, u_int32_t att>
class VertexArray : public Bindable{
public:
	VertexArray();
	VertexArray(const std::array<VertexBuffer<T>, att>& data);
	VertexArray(const VertexArray& other)=delete;
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

/*
 * VERTEX BUFFER METHOD DEFINITIONS
 */

template<typename T>
inline VertexBuffer<T>::VertexBuffer(){
	m_nVert=0;
	m_nDim=0;
	m_size=0;
	m_data=(T*)malloc(0);
	glGenBuffers(1, &m_vbo);
}

template<typename T>
template<int dim>
inline VertexBuffer<T>::VertexBuffer(const Utils::Vec<dim, T>* vert, u_int32_t nVert) : VertexBuffer(){
	upload(vert, nVert);
}

template<typename T>
template<int dim>
inline VertexBuffer<T>::VertexBuffer(const std::vector<Utils::Vec<dim, T>>& vert) : VertexBuffer(){
	upload(vert);
}

template<typename T>
inline VertexBuffer<T>::VertexBuffer(const VertexBuffer<T>& other) : VertexBuffer(){
	upload(other.m_data, other.m_nVert);
}

template<typename T>
inline VertexBuffer<T>::VertexBuffer(VertexBuffer<T>&& other){
	m_nVert=other.m_nVert;
	m_nDim=other.m_nDim;
	m_size=other.m_size;
	m_data=other.m_data;
	m_vbo=other.m_vbo;

	other.m_nVert=0;
	other.m_nDim=0;
	other.m_size=0;
	other.m_data=nullptr;
	other.m_vbo=0;
}

template<typename T>
inline VertexBuffer<T>::~VertexBuffer(){
	free(m_data);
	glDeleteBuffers(1, &m_vbo);
}

template<typename T>
inline void VertexBuffer<T>::bind() const{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

template<typename T>
inline void VertexBuffer<T>::unbind() const{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
template<int dim>
inline void VertexBuffer<T>::upload(const Utils::Vec<dim, T>* vert, u_int32_t nVert){
	if(nVert != m_nVert || dim != m_nDim){
		//Size has changed
		m_nVert=nVert;
		m_nDim=dim;
		m_size=m_nVert * m_nDim * sizeof(T);

		m_data=(T*)realloc(m_data, m_size);
		memcpy(m_data, vert, m_size);
		updateData();
	}else if(memcmp(vert, m_data, m_size)){
		//Contents have changed
		memcpy(m_data, vert, m_size);
		updateData();
	}
}

template<typename T>
template<int dim>
inline void VertexBuffer<T>::upload(const std::vector<Utils::Vec<dim, T>>& vert){
	upload(&vert[0], vert.size());
}

template<typename T>
inline size_t VertexBuffer<T>::getSize() const{
	return m_size;
}

template<typename T>
inline u_int32_t VertexBuffer<T>::getVertNo() const{
	return m_nVert;
}

template<typename T>
inline u_int32_t VertexBuffer<T>::getVertDim() const{
	return m_nDim;
}

template<typename T>
inline void VertexBuffer<T>::updateData(){
	//TODO
}

/*
 * VERTEX ARRAY METHODS
 */

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
