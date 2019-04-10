#pragma once

#include "../Graphics/SharedObject.h"
#include "../Graphics/VertexArray.h"
#include "../Graphics/ShaderUniform.h"
#include "../Graphics/GL/Shader.h"

#include <string>
#include <memory>

namespace Zuazo::Processors{

class ShaderEffect{
public:
	class PassthroughVertexShader : public Graphics::GL::Program::VertexShader{
		PassthroughVertexShader();
		PassthroughVertexShader(const PassthroughVertexShader& other)=delete;
		PassthroughVertexShader(PassthroughVertexShader&& other)=default;
		virtual ~PassthroughVertexShader()=default;
	};

	ShaderEffect(const Graphics::GL::Program& shader, size_t uniformSize);

private:
	class PassthroughVertexArray : public Graphics::VertexArray{
		PassthroughVertexArray();
		PassthroughVertexArray(const PassthroughVertexArray& other)=delete;
		PassthroughVertexArray(PassthroughVertexArray&& other)=default;
		virtual ~PassthroughVertexArray()=default;
	};

	Graphics::SharedObject<PassthroughVertexArray>	m_vertexArray;
	std::unique_ptr<Graphics::ShaderUniform>		m_ubo;
	const Graphics::GL::Program& 					m_shader;
};

}
