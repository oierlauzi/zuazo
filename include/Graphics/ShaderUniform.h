#pragma once

#include "GL/Shader.h"
#include "GL/Buffer.h"
#include "GL/UniqueBinding.h"

#include <sys/types.h>
#include <memory>

namespace Zuazo::Graphics{

class ShaderUniform{
public:
	ShaderUniform(size_t size);
	ShaderUniform(u_int32_t binding, size_t size);
	ShaderUniform(const ShaderUniform& other)=delete;
	ShaderUniform(ShaderUniform&& other)=default;
	~ShaderUniform()=default;

	u_int32_t 							getBinding() const;
	void								setBinding(u_int32_t binding);
	void								bind() const;

	size_t								getSize() const;

	void								setData(const void* data);
	void								setSubset(const void* data, size_t offset, size_t size);

	template<typename T>
	void								setParam(const T& data, size_t offset=0);
private:
	std::unique_ptr<GL::UniformBuffer>	m_uniformBuffer;
	size_t								m_uniformBlockSize;
	u_int32_t							m_binding;
};


inline ShaderUniform::ShaderUniform(size_t size) :
		m_uniformBuffer(new GL::UniformBuffer),
		m_uniformBlockSize(size),
		m_binding(0)
{
	GL::UniqueBinding<GL::UniformBuffer> bufBinding(*m_uniformBuffer);
	GL::UniformBuffer::bufferData(
			m_uniformBlockSize,
			GL::UniformBuffer::Usage::DynamicDraw,
			nullptr
	);
}


inline ShaderUniform::ShaderUniform(u_int32_t binding, size_t size) :
		ShaderUniform(size)
{
	m_binding=binding;
}


inline u_int32_t ShaderUniform::getBinding() const{
	return m_binding;
}


inline void ShaderUniform::setBinding(u_int32_t binding){
	m_binding=binding;
}


inline void ShaderUniform::bind() const{
	glBindBufferRange(
			GL_UNIFORM_BUFFER,		//Target
			m_binding, 				//Index
			m_uniformBuffer->get(),	//Uniform buffer name
			0,						//Offset
			m_uniformBlockSize		//Size
	);
}

inline size_t ShaderUniform::getSize() const{
	return m_uniformBlockSize;
}

inline void ShaderUniform::setData(const void* data){
	GL::UniqueBinding<GL::UniformBuffer> bufBinding(*m_uniformBuffer);
	GL::UniformBuffer::bufferSubData(
			m_uniformBlockSize,
			0,
			data
	);
}

inline void	ShaderUniform::setSubset(const void* data, size_t offset, size_t size){
	GL::UniqueBinding<GL::UniformBuffer> bufBinding(*m_uniformBuffer);
	GL::UniformBuffer::bufferSubData(
			size,
			offset,
			data
	);
}

template<typename T>
inline void ShaderUniform::setParam(const T& data, size_t offset){
	setSubset(&data, offset, sizeof(T));
}
}
