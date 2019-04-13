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
		const std::string& 				getLog() const;
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
	GLint							getUniformCount() const;
	GLint							getUniformIndex(const std::string& name) const;
	std::string						getUniformName(GLint idx) const;
	GLint							getUniformOffset(GLuint idx) const;
	GLint							getUniformOffset(const std::string& name) const;
	GLint							getUniformSize(GLuint idx) const;
	GLint							getUniformSize(const std::string& name) const;
	GLint							getUniformBlock(GLuint idx) const;
	GLint							getUniformBlock(const std::string& name) const;
	GLint							getUniformBlockIndex(const std::string& name) const;
	std::string						getUniformBlockName(GLint idx) const;
	GLint							getUniformBlockSize(GLuint idx) const;
	GLint							getUniformBlockSize(const std::string& name) const;
	void							setUniformBlockBinding(GLuint idx, u_int32_t binding) const;
	void							setUniformBlockBinding(const std::string& name, u_int32_t binding) const;

	GLuint							get() const;
	bool							getSuccess() const;
	const std::string& 				getLog() const;
private:
	GLuint							m_program;
	bool							m_success;
	std::string 					m_log;

	GLint							getUniformParam(GLuint idx, GLenum param) const;
	GLint							getUniformBlockParam(GLuint idx, GLenum param) const;

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
inline const std::string& Program::Shader<type>::getLog() const{
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

inline const std::string& Program::getLog() const{
	return m_log;
}


inline GLint Program::getAttributeLoc(const std::string& name) const{
	return glGetAttribLocation(m_program, name.c_str());
}

inline GLint Program::getUniformCount() const{
	GLint result;
	glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &result);
	return result;
}

inline GLint Program::getUniformIndex(const std::string& name) const{
	return glGetUniformLocation(m_program, name.c_str());
}

inline std::string Program::getUniformName(GLint idx) const{
	GLint size=getUniformParam(idx, GL_UNIFORM_NAME_LENGTH);
	GLchar* charName=(GLchar*)malloc((size + 1) * sizeof(GLchar));

	glGetActiveUniformName(
			m_program,		//Program name
			idx,			//Uniform index
			size,			//Buffer size
			&size,			//Characters written
			charName		//Character buffer
	);

	std::string name(charName);
	free(charName);

	return name;
}

inline GLint Program::getUniformOffset(GLuint idx) const{
	return getUniformParam(idx, GL_UNIFORM_OFFSET);
}

inline GLint Program::getUniformOffset(const std::string& name) const{
	return getUniformOffset(getUniformIndex(name));
}

inline GLint Program::getUniformSize(GLuint idx) const{
	return getUniformParam(idx, GL_UNIFORM_SIZE);
}

inline GLint Program::getUniformSize(const std::string& name) const{
	return getUniformSize(getUniformIndex(name));
}

inline GLint Program::getUniformBlock(GLuint idx) const{
	return getUniformParam(idx, GL_UNIFORM_BLOCK_INDEX);
}

inline GLint Program::getUniformBlock(const std::string& name) const{
	return getUniformBlock(getUniformIndex(name));
}

inline GLint Program::getUniformBlockIndex(const std::string& name) const{
	return glGetUniformBlockIndex(m_program, name.c_str());
}

inline std::string Program::getUniformBlockName(GLint idx) const{
	GLint size=getUniformBlockParam(idx, GL_UNIFORM_BLOCK_NAME_LENGTH);
	GLchar* charName=(GLchar*)malloc((size + 1) * sizeof(GLchar));

	glGetActiveUniformBlockName(
			m_program,		//Program name
			idx,			//Uniform index
			size,			//Buffer size
			&size,			//Characters written
			charName		//Character buffer
	);

	std::string name(charName);
	free(charName);

	return name;
}

inline GLint Program::getUniformBlockSize(GLuint idx) const{
	return getUniformBlockParam(idx, GL_UNIFORM_BLOCK_DATA_SIZE);
}

inline GLint Program::getUniformBlockSize(const std::string& name) const{
	return getUniformBlockSize(getUniformBlockIndex(name));
}

inline void Program::setUniformBlockBinding(GLuint idx, u_int32_t binding) const{
	glUniformBlockBinding(m_program, idx, binding);
}

inline void Program::setUniformBlockBinding(const std::string& name, u_int32_t binding) const{
	setUniformBlockBinding(getUniformBlockIndex(name), binding);
}

inline GLint Program::getUniformParam(GLuint idx, GLenum param) const{
	GLint result;

	glGetActiveUniformsiv(
			m_program,		//Program name
			1,				//Query count
			&idx,			//Uniform indices
			param,			//Query type
			&result			//Query result
	);

	return result;
}

inline GLint Program::getUniformBlockParam(GLuint idx, GLenum param) const{
	GLint result;

	glGetActiveUniformBlockiv(
			m_program,		//Program name
			idx,			//Uniform indices
			param,			//Query type
			&result			//Query result
	);

	return result;
}

}
