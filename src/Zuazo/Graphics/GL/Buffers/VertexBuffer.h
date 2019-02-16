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
class VertexBuffer : public UploadBuffer<GL_ARRAY_BUFFER>{
public:
	VertexBuffer();

	template<u_int32_t dim>
	VertexBuffer(const Utils::Vec<dim, T>* vert, u_int32_t nVert);
	template<u_int32_t dim>
	VertexBuffer(const std::vector<Utils::Vec<dim, T>>& vert);
	template<u_int32_t dim, u_int32_t n>
	VertexBuffer(const std::array<Utils::Vec<dim, T>, n>& vert);
	VertexBuffer(const VertexBuffer<T>& other)=delete;
	VertexBuffer(VertexBuffer<T>&& other);
	~VertexBuffer()=default;

	template<u_int32_t dim>
	void				upload(const Utils::Vec<dim, T>* vert, u_int32_t nVert);
	template<u_int32_t dim>
	void				upload(const std::vector<Utils::Vec<dim, T>>& vert);
	template<u_int32_t dim, u_int32_t n>
	void				upload(const std::array<Utils::Vec<dim, T>, n>& vert);
	void 				upload(const void* data, size_t size)=delete;

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
inline VertexBuffer<T>::VertexBuffer() : UploadBuffer(){
	m_nVert=0;
	m_nDim=0;
}

template<typename T>
template<u_int32_t dim>
inline VertexBuffer<T>::VertexBuffer(const Utils::Vec<dim, T>* vert, u_int32_t nVert) : VertexBuffer(){
	upload(vert, nVert);
}

template<typename T>
template<u_int32_t dim>
inline VertexBuffer<T>::VertexBuffer(const std::vector<Utils::Vec<dim, T>>& vert) : VertexBuffer(){
	upload(vert);
}

template<typename T>
template<u_int32_t dim, u_int32_t n>
inline VertexBuffer<T>::VertexBuffer(const std::array<Utils::Vec<dim, T>, n>& vert) : VertexBuffer(){
	upload(vert);
}

template<typename T>
inline VertexBuffer<T>::VertexBuffer(VertexBuffer<T>&& other) : UploadBuffer(other){
	m_nVert=other.m_nVert;
	m_nDim=other.m_nDim;

	other.m_nVert=0;
	other.m_nDim=0;
}

template<typename T>
template<u_int32_t dim>
inline void VertexBuffer<T>::upload(const Utils::Vec<dim, T>* vert, u_int32_t nVert){
	//TODO
}

template<typename T>
template<u_int32_t dim>
inline void VertexBuffer<T>::upload(const std::vector<Utils::Vec<dim, T>>& vert){
	upload(&vert[0], vert.size());
}

template<typename T>
template<u_int32_t dim, u_int32_t n>
inline void VertexBuffer<T>::upload(const std::array<Utils::Vec<dim, T>, n>& vert){
	upload(&vert[0], n);
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
