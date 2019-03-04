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
	enum class BufferUsage{
		StreamDraw		=GL_STREAM_DRAW,
		StreamRead		=GL_STREAM_READ,
		StreamCopy		=GL_STREAM_COPY,
		StaticDraw		=GL_STATIC_DRAW,
		StaticRead		=GL_STATIC_READ,
		StaticCopy		=GL_STATIC_COPY,
		DynamicWrite	=GL_DYNAMIC_DRAW,
		DynamicRead		=GL_DYNAMIC_READ,
		DynamicCopy		=GL_DYNAMIC_COPY
	};

	class BufferMapping{
	public:
		enum class Access{
			Read			=GL_READ_ONLY,
			Write			=GL_WRITE_ONLY,
			ReadWrite		=GL_READ_WRITE,
		};

		BufferMapping()=delete;
		BufferMapping(Buffer& buf, Access ac);
		BufferMapping(const BufferMapping& other)=delete;
		BufferMapping(BufferMapping&& other);
		~BufferMapping();

		void* 				get() const;
		void				unmap();
	private:
		Buffer& 			buf;
		void*				data;
	};

	Buffer();
	Buffer(const void* data, size_t size);
	Buffer(const Buffer& buf)=delete;
	Buffer(Buffer&& buf);
	virtual ~Buffer();

	void				bind() const;
	void				unbind() const;

	size_t				getSize() const;

	void				allocate(size_t size, BufferUsage usage);
	void				write(const void* data);
	void				read(void* data);
	template<BufferTypes otherType>
	void				copy(const Buffer<otherType>& buf);


private:
	GLuint				m_glBuffer;
	size_t				m_size;
};

typedef Buffer<BufferTypes::PixelUnpack> PixelUnpackBuffer;
typedef Buffer<BufferTypes::Array> VertexArrayBuffer;

/*
 * METHOD DEFINITIONS
 */

template<BufferTypes type>
inline Buffer<type>::Buffer(){
	glGenBuffers(1, &m_glBuffer);
	m_size=0;
}

template<BufferTypes type>
inline Buffer<type>::Buffer(const void* data, size_t size){
	glGenBuffers(1, &m_glBuffer);
	m_size=size;

	UniqueBinding<Buffer<type>> binding(*this);
	glBufferData(
			(GLenum)type,				//target
			m_size,						//buffer size
			data,						//data
			(GLenum)BufferUsage::StaticDraw	//usage
	);
}

template<BufferTypes type>
inline Buffer<type>::Buffer(Buffer<type>&& buf){
	m_glBuffer=buf.m_glBuffer;
	m_size=buf.m_size;

	buf.m_glBuffer=0;
	buf.m_size=0;
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
inline size_t Buffer<type>::getSize() const{
	return m_size;
}

template<BufferTypes type>
inline void Buffer<type>::allocate(size_t size, BufferUsage usage){
	UniqueBinding<Buffer<type>> binding(*this);
	m_size=size;

	//Reallocate data
	glBufferData(
			(GLenum)type,				//target
			m_size,						//buffer size
			nullptr,					//data
			(GLenum)usage				//usage
	);
}

template<BufferTypes type>
inline void Buffer<type>::write(const void* data){
	UniqueBinding<Buffer<type>> binding(*this);
	glBufferSubData(
			(GLenum)type,
			0,
			m_size,
			data
	);
}

template<BufferTypes type>
inline void Buffer<type>::read(void* data){
	UniqueBinding<Buffer<type>> binding(*this);
	glGetBufferSubData(
			(GLenum)type,
			0,
			m_size,
			data
	);
}

template<BufferTypes type>
template<BufferTypes otherType>
inline void Buffer<type>::copy(const Buffer<otherType>& other){
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

template<BufferTypes type>
Buffer<type>::BufferMapping::BufferMapping(Buffer& buf, Access ac) :
	buf(buf)
{
	buf.bind();
	data=glMapBuffer((GLenum)type, (GLenum)ac);
}

template<BufferTypes type>
Buffer<type>::BufferMapping::BufferMapping(BufferMapping&& other) :
	buf(other.buf)
{
	data=other.data;
	other.data=nullptr;
}

template<BufferTypes type>
Buffer<type>::BufferMapping::~BufferMapping(){
	if(data){
		glUnmapBuffer((GLenum)type);
		buf.unbind();
	}
}

template<BufferTypes type>
void* Buffer<type>::BufferMapping::get() const{
	return data;
}

template<BufferTypes type>
void Buffer<type>::BufferMapping::unmap(){
	glUnmapBuffer((GLenum)type);
	buf.unbind();
	data=nullptr;
}
}
