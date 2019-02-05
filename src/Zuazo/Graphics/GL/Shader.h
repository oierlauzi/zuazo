#pragma once

#include <GL/glew.h>
#include <string>

#include "UniqueBinding.h"

namespace Zuazo::Graphics::GL{

class Shader : public Bindable{
public:
	Shader();
	Shader(const Shader& shader)=delete;
	Shader(const std::string&  vertSrc, const std::string& fragSrc);
	~Shader();

	void			bind() const override;
	void			unbind() const override;

	GLint			getUniformLoc(const std::string& name) const;
	GLint			getAttributeLoc(const std::string& name) const;
	GLint			getProgram() const;

	static GLuint 	create(const char*  vertSrc, const char* fragSrc);
	static void		destroy(GLuint shader);
private:
	int				m_program;

	static int 		compile(GLuint shader, const char* src);
};

/*
 * INLINE FUNCTIONS
 */

inline void Shader::bind() const{
	glUseProgram(m_program);
}

inline void Shader::unbind() const{
	glUseProgram(0);
}

inline GLint Shader::getProgram() const{
	return m_program;
}
}
