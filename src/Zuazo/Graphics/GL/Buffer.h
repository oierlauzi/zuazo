#pragma once

#include <glad/glad.h>
#include <stddef.h>
#include <cstring>

#include "UniqueBinding.h"

namespace Zuazo::Graphics::GL{

/*
 * @brief Templated class representing a generic OpenGL buffer
 */
enum class BufferTypes{
	Array				=GL_ARRAY_BUFFER,
	ElementArray		=GL_ELEMENT_ARRAY_BUFFER,
	CopyRead			=GL_COPY_READ_BUFFER,
	CopyWrite			=GL_COPY_WRITE_BUFFER,
	PixelUnpack			=GL_PIXEL_UNPACK_BUFFER,
	PixelPack			=GL_PIXEL_PACK_BUFFER,
	Texture				=GL_TEXTURE_BUFFER,
	TransformFeedback	=GL_TRANSFORM_FEEDBACK_BUFFER,
	Uniform				=GL_UNIFORM_BUFFER,
};

template<BufferTypes type>
class Buffer{
public:
	Buffer();
	Buffer(const Buffer& buf)=delete;
	Buffer(Buffer&& buf);
	~Buffer();

	void				bind() const;
	void				unbind() const;

	GLuint				get() const;
	operator 			GLuint() const;

	static const GLuint	GLType=(GLuint)type;
private:
	GLuint				m_glBuffer;
};

typedef Buffer<BufferTypes::PixelUnpack> PixelUnpackBuffer;
typedef Buffer<BufferTypes::Array> VertexArrayBuffer;

/*
 * METHOD DEFINITIONS
 */

template<BufferTypes type>
inline Buffer<type>::Buffer(){
	glGenBuffers(1, &m_glBuffer);
}

template<BufferTypes type>
inline Buffer<type>::Buffer(Buffer<type>&& buf){
	m_glBuffer=buf.m_glBuffer;
	buf.m_glBuffer=0;
}

template<BufferTypes type>
inline Buffer<type>::~Buffer(){
	glDeleteBuffers(1, &m_glBuffer);
}

template<BufferTypes type>
inline void Buffer<type>::bind() const{
	glBindBuffer((GLenum)type, m_glBuffer);
}

template<BufferTypes type>
inline void Buffer<type>::unbind() const{
	glBindBuffer((GLenum)type, 0);
}

template<BufferTypes type>
inline GLuint Buffer<type>::get() const{
	return m_glBuffer;
}

template<BufferTypes type>
inline Buffer<type>::operator GLuint() const{
	return m_glBuffer;
}
}
