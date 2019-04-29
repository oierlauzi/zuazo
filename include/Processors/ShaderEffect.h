#pragma once

#include "../Graphics/Drawtable.h"
#include "../Graphics/VertexArray.h"
#include "../Graphics/ShaderUniform.h"
#include "../Graphics/GL/Shader.h"
#include "../Video/VideoSourceBase.h"
#include "../Video/VideoConsumerBase.h"
#include "../Video/VideoStream.h"
#include "../Utils/Resolution.h"
#include "../Utils/Buffer.h"

#include <string>
#include <memory>

#define UNIFORM_BLOCK_NAME "shaderFxBlock"
#define VERTEX_ATTRIB_NAME "in_vertices"
#define TEXTURE_COORD_ATTRIB_NAME "in_texCoords"
#define TEXTURE_COORD_EXCHANGE_NAME "ex_texCoords"

namespace Zuazo::Processors{

class ShaderEffect :
	public Video::TVideoSourceBase<Video::LazyVideoSourcePad>,
	public Video::TVideoConsumerBase<Video::VideoConsumerPad>,
	public ZuazoBase
{
public:
	ShaderEffect(const Utils::VideoMode& vidMode, const std::string& fragShader);
	ShaderEffect(const std::string& fragShader);
	ShaderEffect(const ShaderEffect& other)=delete;
	ShaderEffect(ShaderEffect&& other)=default;
	virtual ~ShaderEffect();

	template<typename T>
	void												setParam(const std::string& pname, const T& data);
	template<typename T>
	const T*											getParam(const std::string& pname) const;

	std::string											getShaderLog() const;

	SUPPORTS_GETTING_PIXELFORMAT
	SUPPORTS_SETTING_PIXELFORMAT
	SUPPORTS_LISTING_PIXELFORMATS
	virtual std::set<Utils::PixelFormat>				getSupportedPixelFormats() const override;
	virtual void 										setPixelFormat(const Utils::PixelFormat& pixFmt) override;

	SUPPORTS_GETTING_RESOLUTION

	void												open() override;
	void												close() override;

	void												update() const override;
private:
	struct UniformLayout{
		GLint	index;
		GLenum	type;
		size_t	size;
		size_t	offset;
	};

	std::string											m_fragShaderSrc;
	std::map<std::string, UniformLayout>				m_layout;
	std::unique_ptr<Utils::Buffer>						m_data;

	std::shared_ptr<Graphics::GL::Program>				m_shader;
	std::shared_ptr<Graphics::VertexArray>				m_vertexArray;
	std::unique_ptr<Graphics::ShaderUniform>			m_ubo;
	std::unique_ptr<Graphics::Drawtable>				m_drawtable;
	
	static std::map<std::string, std::weak_ptr<Graphics::VertexArray>> s_vertexArrays;
	static std::map<std::string, std::weak_ptr<Graphics::GL::Program>> s_shaders;
};

template<typename T>
void ShaderEffect::setParam(const std::string& pname, const T& data){
	auto ite=m_layout.find(pname);
	if(ite != m_layout.end()){
		//Requested parameter exists
		const UniformLayout& layout=ite->second;

		//Save the changes on the memory buffer
		memcpy((int8_t*)m_data->get() + layout.offset, &data, sizeof(T));

		if(m_ubo){
			//Also update the data on the uniform buffer
			m_ubo->setParam(data, layout.offset);
		}
	}
}

template<typename T>
const T* ShaderEffect::getParam(const std::string& pname) const{
	auto ite=m_layout.find(pname);
	if(ite != m_layout.end()){
		//Requested parameter exists
		const UniformLayout& layout=ite->second;

		//Return the requested value
		return static_cast<const T*>(m_data->get()) + layout.offset;
	}

	//Failed
	return nullptr;
}

inline std::string ShaderEffect::getShaderLog() const{
	return m_shader ? m_shader->getLog() : "";
}

inline std::set<Zuazo::Utils::PixelFormat> ShaderEffect::getSupportedPixelFormats() const{
	return Graphics::PixelFormat::getSupportedPixelFormats();
}
}
