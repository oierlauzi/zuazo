#pragma once

#include <GL/glew.h>
#include <stddef.h>
#include <cstring>

#include "../UniqueBinding.h"

namespace Zuazo::Graphics::GL::Buffers{

/*
 * @brief Templated class representing a generic OpenGL buffer
 */
template<GLenum type>
class Buffer : public Bindable{
public:
	Buffer();
	Buffer(const void* data, size_t size);
	Buffer(const Buffer<type>& buf)=delete;
	Buffer(Buffer<type>&& buf);
	virtual ~Buffer();

	void				bind() const override;
	void				unbind() const override;

	size_t				getSize() const;
protected:
	GLuint				m_glBuffer;
	size_t				m_size;
};

/*
 * @brief Templated class representing a generic OpenGL buffer that can be used to upload data
 */
template<GLenum type>
class UploadBuffer : public Buffer<type>{
public:
	UploadBuffer()=default;
	UploadBuffer(const void* data, size_t size);
	UploadBuffer(const UploadBuffer<type>& buf)=delete;
	UploadBuffer(UploadBuffer<type>&& buf);
	virtual ~UploadBuffer()=default;

	virtual void 		upload(const void* data, size_t size);
protected:
	using Buffer<type>::m_size;
};

/*
 * BUFFER METHOD DEFINITIONS
 */

template<GLenum type>
inline Buffer<type>::Buffer(){
	glGenBuffers(1, &m_glBuffer);
	m_size=0;
}

template<GLenum type>
inline Buffer<type>::Buffer(Buffer<type>&& buf){
	m_glBuffer=buf.m_glBuffer;
	m_size=buf.m_size;

	buf.m_glBuffer=0;
	buf.m_size=0;
}

template<GLenum type>
inline Buffer<type>::~Buffer(){
	glDeleteBuffers(1, &m_glBuffer);
}

template<GLenum type>
inline void Buffer<type>::bind() const{
	glBindBuffer(type, m_glBuffer);
}

template<GLenum type>
inline void Buffer<type>::unbind() const{
	glBindBuffer(type, 0);
}

template<GLenum type>
inline size_t Buffer<type>::getSize() const{
	return m_size;
}

/*
 * UPLOAD BUFFER METHOD DEFINITIONS
 */

template<GLenum type>
inline UploadBuffer<type>::UploadBuffer(const void* data, size_t size){
	upload(data, size);
}

template<GLenum type>
inline UploadBuffer<type>::UploadBuffer(UploadBuffer<type>&& buf) :
Buffer<type>(static_cast<Buffer<type>&&>(buf)){

}

template<GLenum type>
inline void UploadBuffer<type>::upload(const void* data, size_t size){
	UniqueBinding<UploadBuffer<type>> buf(*this);

	m_size=size;

	//Reallocate data
	glBufferData(
			type,											//target
			m_size,											//buffer size
			nullptr,										//data
			GL_STREAM_DRAW									//usage
	);

	//Map buffer's data onto memory
	void* glData=glMapBuffer(type, GL_WRITE_ONLY);
	if(glData){
		//Copy the data
		memcpy(glData, data, m_size);
		glUnmapBuffer(type);
	}
}
}
