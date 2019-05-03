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

	static void						swizzleMask(const GLint* comp);
	static void						swizzleMask();
	static void						swizzleMask(GLint R, GLint G, GLint B, GLint A);
private:
	GLuint							m_texture;
};

/*
 * Method implementations
 */

inline Texture2D::Texture2D(){
	glGenTextures(1, &m_texture);

	UniqueBinding<Texture2D> binding(*this);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Set up default wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

inline void	Texture2D::swizzleMask(const GLint* comp){
	swizzleMask(
		comp[0],
		comp[1],
		comp[2],
		comp[3]
	);
}

inline void	Texture2D::swizzleMask(){
	swizzleMask(GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA);
}

inline void	Texture2D::swizzleMask(GLint R, GLint G, GLint B, GLint A){
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, R);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, G);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, B);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, A);
}

}
