#pragma once

#include <glad/glad.h>
#include <string>

#include "UniqueBinding.h"

#define ENABLE_SHADER_LOG

namespace Zuazo::Graphics::GL{

class Shader : public Bindable{
public:
	Shader();
	Shader(const Shader& shader)=delete;
	Shader(const std::string&  vertSrc, const std::string& fragSrc);
	~Shader();

	void				bind() const override;
	void				unbind() const override;

	GLint				getUniformLoc(const std::string& name) const;
	GLint				getAttributeLoc(const std::string& name) const;
	GLint				getProgram() const;

#ifdef ENABLE_SHADER_LOG
	const std::string 	getLog();
#endif

private:
	int					m_program;

#ifdef ENABLE_SHADER_LOG
	std::string 		m_log;
#endif
	static int 			compile(GLuint shader, const char* src);
	static int			link(GLuint program, GLuint vertShader, GLuint fragShader);
	static std::string 	getCompilationLog(GLuint shader);
	static std::string 	getLinkLog(GLuint program);
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

#ifdef ENABLE_SHADER_LOG
inline const std::string Shader::getLog(){
	return m_log;
}
#endif
}
