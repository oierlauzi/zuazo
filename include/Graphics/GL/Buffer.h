#pragma once

#include "../../glad/glad.h"
#include <stddef.h>

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
	enum class Usage{
		StreamDraw		=GL_STREAM_DRAW,
		StreamRead		=GL_STREAM_READ,
		StreamCopy		=GL_STREAM_COPY,
		StaticDraw		=GL_STATIC_DRAW,
		StaticRead		=GL_STATIC_READ,
		StaticCopy		=GL_STATIC_COPY,
		DynamicDraw		=GL_DYNAMIC_DRAW,
		DynamicRead		=GL_DYNAMIC_READ,
		DynamicCopy		=GL_DYNAMIC_COPY
	};

	class BufferMapping{
	public:
		enum class Access{
			Read		=GL_READ_ONLY,
			Write		=GL_WRITE_ONLY,
			ReadWrite	=GL_READ_WRITE
		};

		BufferMapping(Access ac);
		BufferMapping(const BufferMapping& other)=delete;
		BufferMapping(BufferMapping&& other);
		~BufferMapping();

		void* 	get() const;
	private:
		void*	m_glData;
	};

	Buffer();
	Buffer(const Buffer& buf)=delete;
	Buffer(Buffer&& buf);
	~Buffer();

	void				bind() const;
	void				unbind() const;

	GLuint				get() const;
	operator 			GLuint() const;

	static const GLuint	GLType=(GLuint)type;

	static void			bufferData(size_t size, Usage usage, const void* data=nullptr);
	static void			bufferSubData(size_t size, size_t off, const void* data=nullptr);
private:
	GLuint				m_glBuffer;
};

typedef Buffer<BufferTypes::PixelUnpack> PixelUnpackBuffer;
typedef Buffer<BufferTypes::Array> VertexArrayBuffer;
typedef Buffer<BufferTypes::Uniform> UniformBuffer;

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
	glBindBuffer(GLType, m_glBuffer);
}

template<BufferTypes type>
inline void Buffer<type>::unbind() const{
	glBindBuffer(GLType, 0);
}

template<BufferTypes type>
inline GLuint Buffer<type>::get() const{
	return m_glBuffer;
}

template<BufferTypes type>
inline Buffer<type>::operator GLuint() const{
	return m_glBuffer;
}

template<BufferTypes type>
inline void	Buffer<type>::bufferData(size_t size, Usage usage, const void* data){
	glBufferData(
			GLType,			//Target
			size,			//Size
			data,			//Ptr to data
			(GLenum)usage	//Usage
	);
}

template<BufferTypes type>
inline void	Buffer<type>::bufferSubData(size_t size, size_t off, const void* data){
	glBufferSubData(
			GLType,			//Target
			off,			//Offset
			size,			//Size
			data			//Ptr to data
	);
}





template<BufferTypes type>
inline Buffer<type>::BufferMapping::BufferMapping(Access ac){
	m_glData=glMapBuffer(
			Buffer<type>::GLType,
			(GLenum)ac
	);
}

template<BufferTypes type>
inline Buffer<type>::BufferMapping::BufferMapping(BufferMapping&& other){
	m_glData=other.m_glData;
	other.m_glData=nullptr;
}

template<BufferTypes type>
inline Buffer<type>::BufferMapping::~BufferMapping(){
	if(m_glData)
		glUnmapBuffer(Buffer<type>::GLType);
}

template<BufferTypes type>
inline void* Buffer<type>::BufferMapping::get() const{
	return m_glData;
}

}
