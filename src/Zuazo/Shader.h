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

	void		use();
	GLint		getUniformLoc(std::string name);
	GLint		getAttributeLoc(std::string name);
private:
	int			m_program;

	static int 	compile(GLuint shader, const char* src);
};

}
