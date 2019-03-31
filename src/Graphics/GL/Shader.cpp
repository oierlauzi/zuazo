#include <Graphics/GL/Shader.h>

using namespace Zuazo::Graphics::GL;

Shader::Shader() {
	m_program=0;
}

Shader::Shader(const std::string&  vertSrc, const std::string& fragSrc){
	//Initialize program to 0
	m_program=0;
	m_log+="*Compiling vertex shader...*\n";


	//Compile the vertex shader
	GLuint vertShader=glCreateShader(GL_VERTEX_SHADER);
	if(compile(vertShader, vertSrc.c_str()) < 0){
		//Error
		m_log+=getCompilationLog(vertShader) + "\n";

		glDeleteShader(vertShader);
		return;
	}

	m_log+="*Compiling fragment shader...*\n";

	//Compile the fragment shader
	GLuint  fragShader=glCreateShader(GL_FRAGMENT_SHADER);
	if(compile(fragShader, fragSrc.c_str()) < 0){
		//Error
		m_log+=getCompilationLog(fragShader) + "\n";

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		return;
	}

	m_log+="*Linking...*\n";


	//Link the program
	GLuint program=glCreateProgram();
	if(link(program, vertShader, fragShader) < 0){
		//Error
		m_log+=getLinkLog(program) + "\n";

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		glDeleteProgram(program);
		return;
	}

	m_log+="*Success!*\n";



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

GLint Shader::getAttributeLoc(const std::string& name) const{
	const auto& ite=m_attribLocations.find(name); //Try to find it in the cache

	GLuint result;
	if(ite!=m_attribLocations.end()){
		result=ite->second;
	}else{
		result=glGetAttribLocation(m_program, name.c_str()); //Cache it for the future
		m_attribLocations.emplace(name, result);
	}

	return result;
}

GLint Shader::getUniformLoc(const std::string& name) const{
	const auto& ite=m_uniformLocations.find(name); //Try to fin it in the cache

	GLuint result;
	if(ite != m_uniformLocations.end()){
		result=ite->second;
	}else{
		result=glGetUniformLocation(m_program, name.c_str()); //Cache it for the future
		m_uniformLocations.emplace(name, result);
	}

	return result;
}

GLint Shader::getUniformBlockIndex(const std::string& name) const{
	const auto& ite=m_uniformBlockLocations.find(name); //Try to fin it in the cache

	GLuint result;
	if(ite != m_uniformBlockLocations.end()){
		result=ite->second;
	}else{
		result=glGetUniformBlockIndex(m_program, name.c_str()); //Cache it for the future
		m_uniformBlockLocations.emplace(name, result);
	}

	return result;
}

void Shader::setUniformBlockBinding(const std::string& name, u_int32_t binding) const{
	u_int32_t idx=getUniformBlockIndex(name);
	glUniformBlockBinding(m_program, idx, binding);
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
