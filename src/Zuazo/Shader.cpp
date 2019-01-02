#include "Shader.h"

#include "Context.h"

using namespace Zuazo;

Shader::Shader() {
	m_program=0;
}

Shader::Shader(const std::string&  vertSrc, const std::string& fragSrc) : Shader(vertSrc.c_str(), fragSrc.c_str()){}

Shader::Shader(const char*  vertSrc, const char* fragSrc){
	m_program=0;

	UniqueContext ctx(Context::mainCtx);

	//compile vertex and fragment shaders
	GLuint vertShader=glCreateShader(GL_VERTEX_SHADER);
	if(compile(vertShader, vertSrc)<0){
		glDeleteShader(vertShader);
		return;
	}

	GLuint  fragShader=glCreateShader(GL_FRAGMENT_SHADER);
	if(compile(fragShader, fragSrc)<0){
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		return;
	}

	//Try to link the program
	m_program=glCreateProgram();
	glAttachShader(m_program, vertShader);
	glAttachShader(m_program, fragShader);
	glLinkProgram(m_program);

	//Delete the shaders
	glDetachShader(m_program, vertShader);
	glDetachShader(m_program, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	//Check if the program has been successfully linked
	int success;
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if(!success){
		glDeleteProgram(m_program);
		m_program=0;
		return;
	}
}


Shader::~Shader() {
	if(m_program){
		UniqueContext ctx(Context::mainCtx);
		glDeleteProgram(m_program);
	}
}

void Shader::use() {
	if(m_program)
		glUseProgram(m_program);
}

GLint Shader::getUniformLoc(std::string name) {
	return glGetUniformLocation(m_program, name.c_str());
}

GLint Shader::getAttributeLoc(std::string name) {
	return glGetAttribLocation(m_program, name.c_str());
}

int Shader::compile(GLuint shader, const char* src) {
	//Compile the shader
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	//Check if the compilation was successful
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if(success)
		return 0; //Compiled successfully
	else
		return -1; //Compilation error
}
