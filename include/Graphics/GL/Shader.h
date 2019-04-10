#pragma once

#include "../../glad/glad.h"

#include <string>
#include <map>

namespace Zuazo::Graphics::GL{

class Program{
public:
	template <GLenum type>
	class Shader{
	public:
		Shader(const std::string& src);
		Shader(const Shader& other)=delete;
		Shader(Shader&& other);
		virtual ~Shader();

		GLuint							get() const;
		bool							getSuccess() const;
		const std::string 				getLog() const;
	private:
		GLuint							m_shader;
		bool							m_success;
		std::string						m_log;
	};

	typedef Shader<GL_VERTEX_SHADER> VertexShader;
	typedef Shader<GL_FRAGMENT_SHADER> FragmentShader;

	Program(const Program& other)=delete;
	Program(Program&& other);
	Program(const std::string&  vertSrc, const std::string& fragSrc);
	Program(const VertexShader& vert, const FragmentShader& frag);
	virtual ~Program();

	void							bind() const;
	void							unbind() const;

	GLint							getAttributeLoc(const std::string& name) const;
	GLint							getUniformLoc(const std::string& name) const;
	GLint							getUniformBlockIndex(const std::string& name) const;
	void							setUniformBlockBinding(const std::string& name, u_int32_t binding) const;

	GLuint							get() const;
	bool							getSuccess() const;
	const std::string 				getLog() const;
private:
	GLuint							m_program;
	bool							m_success;
	std::string 					m_log;

	mutable std::map<std::string, GLuint> m_attribLocations;
	mutable std::map<std::string, GLuint> m_uniformLocations;
	mutable std::map<std::string, GLuint> m_uniformBlockLocations;

	static GLuint					linkShaders(const VertexShader& vert, const FragmentShader& frag);
	static bool						getLinkSuccess(GLuint program);
	static std::string				getLinkLog(GLuint program);
};

/*
 * INLINE FUNCTIONS
 */

template <GLenum type>
inline Program::Shader<type>::Shader(const std::string& src) :
	m_shader(glCreateShader(type))
{
	const char* shaderSource=src.c_str();
	glShaderSource(m_shader, 1, &shaderSource, NULL);
	glCompileShader(m_shader);

	//Query the size of the log
	GLint len = 0;
	glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &len);

	//Check if successfully compiled
	int success;
	glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
	m_success=(success == GL_TRUE);

	//Query the log
	GLchar* log=new GLchar[len];
	glGetShaderInfoLog(m_shader, len, &len, log);

	m_log=std::string(log);
	delete[] log;
}

template <GLenum type>
inline Program::Shader<type>::Shader(Shader&& other) :
	m_shader(other.m_shader),
	m_success(other.m_success),
	m_log(std::move(other.m_log))
{
	other.m_shader=0;
}

template <GLenum type>
inline Program::Shader<type>::~Shader(){
	if(m_shader)
		glDeleteShader(m_shader);
}

template <GLenum type>
inline GLuint Program::Shader<type>::get() const{
	return m_shader;
}

template <GLenum type>
inline bool Program::Shader<type>::getSuccess() const{
	return m_success;
}

template <GLenum type>
inline const std::string Program::Shader<type>::getLog() const{
	return m_log;
}



inline void Program::bind() const{
	glUseProgram(m_program);
}

inline void Program::unbind() const{
	glUseProgram(0);
}

inline GLuint Program::get() const{
	return m_program;
}

inline bool Program::getSuccess() const{
	return m_success;
}

inline const std::string Program::getLog() const{
	return m_log;
}

}
