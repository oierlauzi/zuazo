#pragma once

#include <GL/glew.h>
#include <string>

namespace Zuazo{

class Shader {
public:
	Shader();
	Shader(const Shader& shader)=delete;
	Shader(const std::string&  vertSrc, const std::string& fragSrc);
	Shader(const char*  vertSrc, const char* fragSrc);
	~Shader();

	void		use() const;
	static void unuse();
	GLint		getUniformLoc(const std::string& name) const;
	GLint		getAttributeLoc(const std::string& name) const;

	static GLuint create(const char*  vertSrc, const char* fragSrc);
	static void	destroy(GLuint shader);
private:
	int			m_program;

	static int 	compile(GLuint shader, const char* src);
};

class UniqueShader{
public:
	UniqueShader(const Shader& shader);
	UniqueShader(const Shader* shader);
	UniqueShader(const UniqueShader& shader)=delete;
	~UniqueShader();
};

/*
 * INLINE FUNCTIONS
 */

inline void Shader::use() const{
	glUseProgram(m_program);
}

inline void Shader::unuse(){
	glUseProgram(0);
}



inline UniqueShader::UniqueShader(const Shader& shader){
	shader.use();
}

inline UniqueShader::UniqueShader(const Shader* shader){
	shader->use();
}

inline UniqueShader::~UniqueShader(){
	Shader::unuse();
}
}
