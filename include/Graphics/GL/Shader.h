#pragma once

#include "../../glad/glad.h"

#include <string>

namespace Zuazo::Graphics::GL{

class Shader{
public:
	Shader();
	Shader(const Shader& shader)=delete;
	Shader(const std::string&  vertSrc, const std::string& fragSrc);
	~Shader();

	void				bind() const;
	void				unbind() const;

	GLint				getUniformLoc(const std::string& name) const;
	GLint				getAttributeLoc(const std::string& name) const;
	GLint				getProgram() const;

	const std::string 	getLog();
private:
	int					m_program;

	std::string 		m_log;

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
