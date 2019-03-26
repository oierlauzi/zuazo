#pragma once

#include "../../glad/glad.h"

namespace Zuazo::Graphics::GL{

class FrameBuffer{
public:
	FrameBuffer();
	FrameBuffer(const FrameBuffer& other)=delete;
	FrameBuffer(FrameBuffer&& other);
	~FrameBuffer();

	void			bind() const;
	void			unbind() const;
private:
	GLuint			m_fbo;
};

/*
 * FrameBuffer's inline methods
 */

inline FrameBuffer::FrameBuffer(){
	glGenFramebuffers(1, &m_fbo);
}

inline FrameBuffer::FrameBuffer(FrameBuffer&& other){
	m_fbo=other.m_fbo;
	other.m_fbo=0;
}

inline FrameBuffer::~FrameBuffer(){
	glDeleteFramebuffers(1, &m_fbo);
}

inline void FrameBuffer::bind() const{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

inline void FrameBuffer::unbind() const{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
}

