#pragma once

#include <glad/glad.h>
#include <sys/types.h>

#include "Texture.h"
#include "UniqueBinding.h"

namespace Zuazo::Graphics::GL{

class FrameBuffer{
public:
	enum class Target{
		READ=GL_READ_FRAMEBUFFER,
		DRAW=GL_DRAW_FRAMEBUFFER,
		BOTH=GL_FRAMEBUFFER
	};

	FrameBuffer();
	FrameBuffer(const FrameBuffer& other)=delete;
	~FrameBuffer();

	void			bind(Target trgt) const;
	void			unbind(Target trgt) const;
	void			bind() const;
	void			unbind() const;

	template <u_int32_t no=0>
	void			attach(Texture& tex);
	template <u_int32_t no=0>
	void			detach(Texture& tex);
private:
	GLuint			m_fbo;
};

template <typename T>
class UniqueAttachment{
public:
	UniqueAttachment(FrameBuffer& fbo, T& trgt);
	UniqueAttachment(const UniqueAttachment& other)=delete;
	~UniqueAttachment();
private:
	FrameBuffer&	m_fbo;
	T&				m_target;
};

/*
 * FrameBuffer's inline methods
 */

inline FrameBuffer::FrameBuffer(){
	glGenFramebuffers(1, &m_fbo);
}

inline FrameBuffer::~FrameBuffer(){
	glDeleteFramebuffers(1, &m_fbo);
}


inline void FrameBuffer::bind(Target trgt) const{
	glBindFramebuffer((GLenum)trgt, m_fbo);
}

inline void FrameBuffer::unbind(Target trgt) const{
	glBindFramebuffer((GLenum)trgt, 0);
}

inline void FrameBuffer::bind() const{
	bind(Target::BOTH);
}

inline void FrameBuffer::unbind() const{
	unbind(Target::BOTH);
}

template <u_int32_t no>
inline void FrameBuffer::attach(Texture& tex){
	UniqueBinding<FrameBuffer> binding(*this);

	glFramebufferTexture2D(
			(GLenum)Target::BOTH,
			GL_COLOR_ATTACHMENT0 + no,
			GL_TEXTURE_2D,
			tex.m_texture,
			0);
}

template <u_int32_t no>
inline void FrameBuffer::detach(Texture& tex){
	UniqueBinding<FrameBuffer> binding(*this);

	glFramebufferTexture2D(
			(GLenum)Target::BOTH,
			GL_COLOR_ATTACHMENT0 + no,
			GL_TEXTURE_2D,
			0,
			0);
}

/*
 * UniqueAttachment's methods
 */

template <typename T>
inline UniqueAttachment<T>::UniqueAttachment(FrameBuffer& fbo, T& trgt) :
		m_fbo(fbo),
		m_target(trgt){

	m_fbo.attach(m_target);

}

template <typename T>
inline UniqueAttachment<T>::~UniqueAttachment(){
	m_fbo.detach(m_target);
}

}

