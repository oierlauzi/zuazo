#pragma once

#include "GL/Shader.h"
#include "GL/Buffer.h"
#include "GL/UniqueBinding.h"

#include <sys/types.h>
#include <memory>

namespace Zuazo::Graphics{

template <typename T>
constexpr size_t uniformSize=sizeof(T);

template <typename T>
class ShaderUniform{
public:
	ShaderUniform();
	ShaderUniform(u_int32_t binding);
	ShaderUniform(const ShaderUniform& other)=delete;
	ShaderUniform(ShaderUniform&& other)=default;
	~ShaderUniform()=default;

	u_int32_t 							getBinding() const;
	void								setBinding(u_int32_t binding);
	void								bind() const;

	void								setData(const T& data);
	void								setSubset(const void* data, size_t offset, size_t size);
private:
	std::unique_ptr<GL::UniformBuffer>	m_uniformBuffer;
	u_int32_t							m_binding;
};

template <typename T>
inline ShaderUniform<T>::ShaderUniform() :
		m_uniformBuffer(new GL::UniformBuffer),
		m_binding(0)
{
	GL::UniqueBinding<GL::UniformBuffer> bufBinding(*m_uniformBuffer);
	GL::UniformBuffer::bufferData(
			uniformSize<T>,
			GL::UniformBuffer::Usage::DynamicDraw,
			nullptr
	);
}

template <typename T>
inline ShaderUniform<T>::ShaderUniform(u_int32_t binding) :
		ShaderUniform()
{
	m_binding=binding;
}

template <typename T>
inline u_int32_t ShaderUniform<T>::getBinding() const{
	return m_binding;
}

template <typename T>
inline void ShaderUniform<T>::setBinding(u_int32_t binding){
	m_binding=binding;
}

template <typename T>
inline void ShaderUniform<T>::bind() const{
	glBindBufferRange(
			GL_UNIFORM_BUFFER,		//Target
			m_binding, 				//Index
			m_uniformBuffer->get(),	//Uniform buffer name
			0,						//Offset
			uniformSize<T>			//Size
	);
}

template <typename T>
inline void ShaderUniform<T>::setData(const T& data){
	GL::UniqueBinding<GL::UniformBuffer> bufBinding(*m_uniformBuffer);
	GL::UniformBuffer::bufferSubData(
			sizeof(T),
			0,
			&data
	);
}

template <typename T>
inline void	ShaderUniform<T>::setSubset(const void* data, size_t offset, size_t size){
	GL::UniqueBinding<GL::UniformBuffer> bufBinding(*m_uniformBuffer);
	GL::UniformBuffer::bufferSubData(
			size,
			offset,
			data
	);
}
}
