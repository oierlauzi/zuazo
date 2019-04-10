#include <Graphics/GL/Shader.h>

using namespace Zuazo::Graphics::GL;

Program::Program(const std::string&  vertSrc, const std::string& fragSrc) :
		Program(VertexShader(vertSrc), FragmentShader(fragSrc))
{
}

Program::Program(const VertexShader& vert, const FragmentShader& frag){
	//Initialize program to 0
	m_program=0;

	//Check if the vertex shader has been successfully compiled
	m_log+="*VERTEX SHADER COMPILATION RESULT:*\n\n";
	m_log+=vert.getLog();
	if(!vert.getSuccess()){
		m_success=false;
		return;
	}

	//Check if the fragment shader has been successfully compiled
	m_log+="*FRAGMENT SHADER COMPILATION RESULT:*\n\n";
	m_log+=frag.getLog();
	if(!frag.getSuccess()){
		m_success=false;
		return;
	}

	//Link the program
	m_program=linkShaders(vert, frag);

	m_log+="*PROGRAM LINKAGE RESULT:*\n\n";
	m_log+=getLinkLog(m_program);
	if(!getLinkSuccess(m_program)){
		m_success=false;
		glDeleteProgram(m_program);
		m_program=0;
		return;
	}

	m_log+="*SUCCESS!*\n";
}

Program::Program(Program&& other) :
	m_program(other.m_program),
	m_success(other.m_success),
	m_log(std::move(other.m_log)),

	m_attribLocations(std::move(other.m_attribLocations)),
	m_uniformLocations(std::move(other.m_uniformLocations)),
	m_uniformBlockLocations(std::move(other.m_uniformBlockLocations))
{
	other.m_program=0;
}

Program::~Program() {
	if(m_program){
		glDeleteProgram(m_program);
	}
}

GLint Program::getAttributeLoc(const std::string& name) const{
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

GLint Program::getUniformLoc(const std::string& name) const{
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

GLint Program::getUniformBlockIndex(const std::string& name) const{
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

void Program::setUniformBlockBinding(const std::string& name, u_int32_t binding) const{
	u_int32_t idx=getUniformBlockIndex(name);
	glUniformBlockBinding(m_program, idx, binding);
}

GLuint Program::linkShaders(const VertexShader& vert, const FragmentShader& frag){
	//Create the program and link the shaders
	GLuint program=glCreateProgram();

	//Attach the shaders
	glAttachShader(program, vert.get());
	glAttachShader(program, frag.get());

	//Link the program
	glLinkProgram(program);

	//Detach the shaders
	glDetachShader(program, vert.get());
	glDetachShader(program, frag.get());

	return program;
}

bool Program::getLinkSuccess(GLuint program){
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	return (success == GL_TRUE);
}

std::string Program::getLinkLog(GLuint program) {
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
