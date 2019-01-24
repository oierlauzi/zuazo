#include "Shader.h"

#include "Context.h"

using namespace Zuazo::Graphics::GL;

Shader::Shader() {
	m_program=0;
}

Shader::Shader(const std::string&  vertSrc, const std::string& fragSrc) : Shader(vertSrc.c_str(), fragSrc.c_str()){}

Shader::Shader(const char*  vertSrc, const char* fragSrc){
	UniqueContext ctx(Context::mainCtx);
	m_program=create(vertSrc, fragSrc);
}


Shader::~Shader() {
	if(m_program){
		UniqueContext ctx(Context::mainCtx);
		destroy(m_program);
	}
}

GLint Shader::getUniformLoc(const std::string& name) const{
	return glGetUniformLocation(m_program, name.c_str());
}

GLint Shader::getAttributeLoc(const std::string& name) const{
	return glGetAttribLocation(m_program, name.c_str());
}



GLuint Shader::create(const char*  vertSrc, const char* fragSrc){
	GLuint program=0;

	//compile vertex and fragment shaders
	GLuint vertShader=glCreateShader(GL_VERTEX_SHADER);
	if(compile(vertShader, vertSrc)<0){
		glDeleteShader(vertShader);
		return 0;
	}

	GLuint  fragShader=glCreateShader(GL_FRAGMENT_SHADER);
	if(compile(fragShader, fragSrc)<0){
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		return 0;
	}

	//Try to link the program
	program=glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	//Delete the shaders
	glDetachShader(program, vertShader);
	glDetachShader(program, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	//Check if the program has been successfully linked
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success){
		glDeleteProgram(program);
		return 0;
	}

	return program;
}

void Shader::destroy(GLuint shader){
	glDeleteProgram(shader);
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
