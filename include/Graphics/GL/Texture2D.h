#pragma once

#include "../../glad/glad.h"
#include "UniqueBinding.h"

#include <sys/types.h>

namespace Zuazo::Graphics::GL{

class Texture2D{
public:
	Texture2D();
	Texture2D(const Texture2D& other)=delete;
	Texture2D(Texture2D&& other);
	~Texture2D();

	void							bind() const;
	void							bind(u_int32_t no) const;
	void							unbind() const;
	void							unbind(u_int32_t no) const;

	GLuint							get() const;
private:
	GLuint							m_texture;
};

/*
 * Method implementations
 */

inline Texture2D::Texture2D(){
	glGenTextures(1, &m_texture);

	UniqueBinding<Texture2D> binding(*this);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Set up default wrapping
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

inline Texture2D::Texture2D(Texture2D&& other){
	m_texture=other.m_texture;
	other.m_texture=0;
}


inline Texture2D::~Texture2D(){
	glDeleteTextures(1, &m_texture);
}

inline void	Texture2D::bind() const{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

inline void	Texture2D::bind(u_int32_t no) const{
	glActiveTexture(GL_TEXTURE0 + no);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

inline void	Texture2D::unbind() const{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

inline void	Texture2D::unbind(u_int32_t no) const{
	glActiveTexture(GL_TEXTURE0 + no);
	glBindTexture(GL_TEXTURE_2D, 0);
}

inline GLuint Texture2D::get() const{
	return m_texture;
}
}
