#pragma once

#include "../../glad/glad.h"
#include "UniqueBinding.h"

#include <sys/types.h>

namespace Zuazo::Graphics::GL{

class RenderBuffer{
public:
	RenderBuffer();
	RenderBuffer(const RenderBuffer& other)=delete;
	RenderBuffer(RenderBuffer&& other);
	~RenderBuffer();

	void							bind() const;
	void							unbind() const;

	GLuint							get() const;

	static void						allocate(u_int32_t width, u_int32_t height, GLenum fmt);
private:
	GLuint							m_rbo;
};

/*
 * Method implementations
 */

inline RenderBuffer::RenderBuffer(){
	glGenRenderbuffers(1, &m_rbo);
}

inline RenderBuffer::RenderBuffer(RenderBuffer&& other){
	m_rbo=other.m_rbo;
	other.m_rbo=0;
}


inline RenderBuffer::~RenderBuffer(){
	glDeleteRenderbuffers(1, &m_rbo);
}

inline void	RenderBuffer::bind() const{
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
}


inline void	RenderBuffer::unbind() const{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

inline void	RenderBuffer::allocate(u_int32_t width, u_int32_t height, GLenum fmt){
	glRenderbufferStorage(
			GL_RENDERBUFFER,		//Target
			fmt,					//Internal format
			width,					//Width
			height					//Height
	);
}

inline GLuint RenderBuffer::get() const{
	return m_rbo;
}
}
