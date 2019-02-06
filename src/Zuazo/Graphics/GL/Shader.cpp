#include "Shader.h"

#include "Context.h"

using namespace Zuazo::Graphics::GL;

Shader::Shader() {
	m_program=0;
}

Shader::Shader(const std::string&  vertSrc, const std::string& fragSrc){
	//Initialize program to 0
	m_program=0;

	#ifdef ENABLE_SHADER_LOG
	m_log+="*Compiling vertex shader...*\n";
	#endif

	//Compile the vertex shader
	GLuint vertShader=glCreateShader(GL_VERTEX_SHADER);
	if(compile(vertShader, vertSrc.c_str()) < 0){
		//Error

		#ifdef ENABLE_SHADER_LOG
		m_log+=getCompilationLog(vertShader) + "\n";
		#endif

		glDeleteShader(vertShader);
		return;
	}

	#ifdef ENABLE_SHADER_LOG
	m_log+="*Compiling fragment shader...*\n";
	#endif

	//Compile the fragment shader
	GLuint  fragShader=glCreateShader(GL_FRAGMENT_SHADER);
	if(compile(fragShader, fragSrc.c_str()) < 0){
		//Error

		#ifdef ENABLE_SHADER_LOG
		m_log+=getCompilationLog(fragShader) + "\n";
		#endif

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		return;
	}

	#ifdef ENABLE_SHADER_LOG
	m_log+="*Linking...*\n";
	#endif

	//Link the program
	GLuint program=glCreateProgram();
	if(link(program, vertShader, fragShader) < 0){
		//Error

		#ifdef ENABLE_SHADER_LOG
		m_log+=getLinkLog(program) + "\n";
		#endif

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		glDeleteProgram(program);
		return;
	}

	#ifdef ENABLE_SHADER_LOG
	m_log+="*Success!*\n";
	#endif


	//Successfuly compiled and linked shaders
	//Delete shaders (not needed)
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	//Assign progam
	m_program=program;
}


Shader::~Shader() {
	if(m_program){
		glDeleteProgram(m_program);
	}
}

GLint Shader::getUniformLoc(const std::string& name) const{
	return glGetUniformLocation(m_program, name.c_str());
}

GLint Shader::getAttributeLoc(const std::string& name) const{
	return glGetAttribLocation(m_program, name.c_str());
}

int Shader::compile(GLuint shader, const char* src) {
	//Compile the shader
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	//Check if the compilation was successful
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if(success == GL_FALSE)
		return -1; //Compilation error
	else
		return 0; //Compiled successfully
}

int Shader::link(GLuint program, GLuint vertShader, GLuint fragShader){
	//Attach the shaders
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);

	//Link the program
	glLinkProgram(program);

	//Detach the shaders
	glDetachShader(program, vertShader);
	glDetachShader(program, fragShader);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(success == GL_FALSE){
		return -1;
	}else
		return 0;
}

std::string Shader::getCompilationLog(GLuint shader) {
	//Query the size of the log
	GLint len = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

	//Query the log
	GLchar* log=new GLchar[len];
	glGetShaderInfoLog(shader, len, &len, log);

	std::string strLog(log);
	delete[] log;

	return strLog;
}

std::string Shader::getLinkLog(GLuint program) {
	//Query the size of the log
	GLint len = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

	//Query the log
	GLchar* log=new GLchar[len];
	glGetProgramInfoLog(program, len, &len, log);

	std::string strLog(log);
	delete[] log;

	return strLog;
}
