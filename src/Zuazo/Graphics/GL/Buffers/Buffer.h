#pragma once

#include <glad/glad.h>
#include <stddef.h>
#include <cstring>

#include "../UniqueBinding.h"

namespace Zuazo::Graphics::GL::Buffers{

/*
 * @brief Templated class representing a generic OpenGL buffer
 */
template<GLenum type>
class BufferBase : public Bindable{
public:
	BufferBase();
	BufferBase(const BufferBase<type>& buf)=delete;
	BufferBase(BufferBase<type>&& buf);
	virtual ~BufferBase();

	void				bind() const override;
	void				unbind() const override;

	size_t				getSize() const;

protected:
	void				allocate(size_t size, GLenum usage);
	void				write(const void* data);
	void				read(void* data);
	template<GLenum otherType>
	void				copy(const BufferBase<otherType>& buf);
private:
	GLuint				m_glBuffer;
	size_t				m_size;
};

template<GLenum type>
class Buffer : public BufferBase<type>{
public:
	using BufferBase<type>::BufferBase;
	using BufferBase<type>::allocate;
	using BufferBase<type>::write;
	using BufferBase<type>::read;
};

/*
 * METHOD DEFINITIONS
 */

template<GLenum type>
inline BufferBase<type>::BufferBase(){
	glGenBuffers(1, &m_glBuffer);
	m_size=0;
}

template<GLenum type>
inline BufferBase<type>::BufferBase(BufferBase<type>&& buf){
	m_glBuffer=buf.m_glBuffer;
	m_size=buf.m_size;

	buf.m_glBuffer=0;
	buf.m_size=0;
}

template<GLenum type>
inline BufferBase<type>::~BufferBase(){
	glDeleteBuffers(1, &m_glBuffer);
}

template<GLenum type>
inline void BufferBase<type>::bind() const{
	glBindBuffer(type, m_glBuffer);
}

template<GLenum type>
inline void BufferBase<type>::unbind() const{
	glBindBuffer(type, 0);
}

template<GLenum type>
inline size_t BufferBase<type>::getSize() const{
	return m_size;
}

template<GLenum type>
inline void BufferBase<type>::allocate(size_t size, GLenum usage){
	UniqueBinding<BufferBase<type>> binding(*this);
	m_size=size;

	//Reallocate data
	glBufferData(
			type,						//target
			m_size,						//buffer size
			nullptr,					//data
			usage						//usage
	);
}

template<GLenum type>
inline void BufferBase<type>::write(const void* data){
	UniqueBinding<BufferBase<type>> binding(*this);

	//Map the contents onto memory
	void* glData=glMapBuffer(type, GL_WRITE_ONLY);
	if(glData){
		//Copy the data
		memcpy(glData, data, m_size);
		glUnmapBuffer(type);
	}
}

template<GLenum type>
inline void BufferBase<type>::read(void* data){
	UniqueBinding<BufferBase<type>> binding(*this);

	//Map the contents onto memory
	const void* glData=glMapBuffer(type, GL_READ_ONLY);
	if(glData){
		//Copy the data
		memcpy(data, glData, m_size);
		glUnmapBuffer(type);
	}
}

template<GLenum type>
template<GLenum otherType>
inline void BufferBase<type>::copy(const BufferBase<otherType>& other){
	//Bind the buffers. Not using RAII style because only one
	// buffer can be bound to the same target
	glBindBuffer(GL_COPY_READ_BUFFER, other.m_glBuffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, m_glBuffer);

	glCopyBufferSubData(
			GL_COPY_READ_BUFFER,		//read target
			GL_COPY_WRITE_BUFFER,		//write target
			0,							//read offset
			0,							//write offset
			m_size						//size
	);

	//Unbind the buffers
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}
}
