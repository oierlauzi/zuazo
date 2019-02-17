#pragma once

#include <GL/glew.h>
#include <stddef.h>
#include <sys/types.h>
#include <array>
#include <vector>

#include "../../../Utils/Vector.h"
#include "Buffer.h"

namespace Zuazo::Graphics::GL::Buffers{

template<typename T>
class VertexBuffer : public BufferBase<GL_ARRAY_BUFFER>{
public:
	VertexBuffer();

	template<int dim>
	VertexBuffer(const Utils::Vec<dim, T>* vert, u_int32_t nVert);

	template<int dim>
	VertexBuffer(const std::vector<Utils::Vec<dim, T>>& vert);

	template<int dim, size_t n>
	VertexBuffer(const std::array<Utils::Vec<dim, T>, n>& vert);

	VertexBuffer(const VertexBuffer& other)=delete;
	VertexBuffer(VertexBuffer&& other);
	~VertexBuffer()=default;

	template<int dim>
	void				upload(const Utils::Vec<dim, T>* vert, u_int32_t nVert);
	template<int dim>
	void				upload(const std::vector<Utils::Vec<dim, T>>& vert);
	template<int dim, size_t n>
	void				upload(const std::array<Utils::Vec<dim, T>, n>& vert);

	u_int32_t			getVertNo() const;
	u_int32_t			getVertDim() const;
private:
	u_int32_t			m_nVert;
	u_int32_t			m_nDim;
};

/*
 * METHOD DEFINITIONS
 */

template<typename T>
inline VertexBuffer<T>::VertexBuffer() : BufferBase<GL_ARRAY_BUFFER>(){
	m_nVert=0;
	m_nDim=0;
}

template<typename T>
template<int dim>
inline VertexBuffer<T>::VertexBuffer(const Utils::Vec<dim, T>* vert, u_int32_t nVert) : VertexBuffer(){
	m_nVert=nVert;
	m_nDim=dim;
	size_t size=m_nVert * m_nDim * sizeof(T);

	BufferBase<GL_ARRAY_BUFFER>::allocate(size, GL_STATIC_DRAW);
	BufferBase<GL_ARRAY_BUFFER>::write(vert);
}

template<typename T>
template<int dim>
inline VertexBuffer<T>::VertexBuffer(const std::vector<Utils::Vec<dim, T>>& vert) : VertexBuffer(&(vert[0]), vert.size()){
	upload(vert);
}

template<typename T>
template<int dim, size_t n>
inline VertexBuffer<T>::VertexBuffer(const std::array<Utils::Vec<dim, T>, n>& vert) : VertexBuffer(&(vert[0]), vert.size()){
	upload(vert);
}

template<typename T>
inline VertexBuffer<T>::VertexBuffer(VertexBuffer&& other) :
	BufferBase<GL_ARRAY_BUFFER>(static_cast<BufferBase<GL_ARRAY_BUFFER>&&>(other))
{
	m_nVert=other.m_nVert;
	m_nDim=other.m_nDim;

	other.m_nVert=0;
	other.m_nDim=0;
}

template<typename T>
template<int dim>
inline void VertexBuffer<T>::upload(const Utils::Vec<dim, T>* vert, u_int32_t nVert){
	m_nVert=nVert;
	m_nDim=dim;
	size_t size=m_nVert * m_nDim * sizeof(T);

	BufferBase<GL_ARRAY_BUFFER>::allocate(size, GL_STREAM_DRAW);
	BufferBase<GL_ARRAY_BUFFER>::write(vert);
}

template<typename T>
template<int dim>
inline void VertexBuffer<T>::upload(const std::vector<Utils::Vec<dim, T>>& vert){
	upload(&(vert[0]), vert.size());
}

template<typename T>
template<int dim, size_t n>
inline void VertexBuffer<T>::upload(const std::array<Utils::Vec<dim, T>, n>& vert){
	upload(&(vert[0]), vert.size());
}

template<typename T>
inline u_int32_t VertexBuffer<T>::getVertNo() const{
	return m_nVert;
}

template<typename T>
inline u_int32_t VertexBuffer<T>::getVertDim() const{
	return m_nDim;
}
}
