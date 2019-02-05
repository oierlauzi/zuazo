#pragma once

#include <GL/glew.h>
#include <sys/types.h>

#include "Texture2D.h"
#include "UniqueBinding.h"

namespace Zuazo::Graphics::GL{

class FrameBuffer : public Bindable{
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
	void			bind() const override;
	void			unbind() const override;

	void			attach(Texture2D& tex);
	void			detach(Texture2D& tex);
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

template <typename T, u_int32_t no>
class UniqueAttachment<T>;

/*
 * FrameBuffer's inline methods
 */

inline FrameBuffer::FrameBuffer(){
	glCreateFramebuffers(1, &m_fbo);
}

inline FrameBuffer::FrameBuffer(){
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

inline void FrameBuffer::attach(Texture2D& tex){
	bind(Target::BOTH);

	glFramebufferTexture2D(
			(GLenum)Target::BOTH,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			tex.m_tex,
			0);

	unbind(Target::BOTH);
}

inline void FrameBuffer::detach(Texture2D& tex){
	bind(Target::BOTH);

	glFramebufferTexture2D(
			(GLenum)Target::BOTH,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			0,
			0);

	unbind(Target::BOTH);
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

template <typename T, u_int32_t no>
inline UniqueAttachment<T>::UniqueAttachment(FrameBuffer& fbo, T& trgt) :
		m_fbo(fbo),
		m_target(trgt){

	m_fbo.attach(m_target, no);

}

template <typename T>
inline UniqueAttachment<T>::~UniqueAttachment(){
	m_fbo.detach(m_target);
}

template <typename T, u_int32_t no>
inline UniqueAttachment<T>::~UniqueAttachment(){
	m_fbo.detach(m_target, no);
}

}

