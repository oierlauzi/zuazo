#pragma once

#include "../../glad/glad.h"

#include <string>
#include <map>

namespace Zuazo::Graphics::GL{

class Shader{
public:
	Shader();
	Shader(const Shader& shader)=delete;
	Shader(const std::string&  vertSrc, const std::string& fragSrc);
	virtual ~Shader();

	void							bind() const;
	void							unbind() const;

	GLint							getAttributeLoc(const std::string& name) const;
	GLint							getUniformLoc(const std::string& name) const;
	GLint							getUniformBlockIndex(const std::string& name) const;
	void							setUniformBlockBinding(const std::string& name, u_int32_t binding) const;
	GLint							get() const;

	const std::string 				getLog() const;
private:
	int								m_program;

	mutable std::map<std::string, GLuint> m_attribLocations;
	mutable std::map<std::string, GLuint> m_uniformLocations;
	mutable std::map<std::string, GLuint> m_uniformBlockLocations;

	std::string 					m_log;

	static int 						compile(GLuint shader, const char* src);
	static int						link(GLuint program, GLuint vertShader, GLuint fragShader);
	static std::string 				getCompilationLog(GLuint shader);
	static std::string 				getLinkLog(GLuint program);
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

inline GLint Shader::get() const{
	return m_program;
}

inline const std::string Shader::getLog() const{
	return m_log;
}

}
