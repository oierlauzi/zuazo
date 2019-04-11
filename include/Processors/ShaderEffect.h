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

namespace Zuazo::Processors{


class ShaderEffect :
	public Video::TVideoSourceBase<Video::LazyVideoSourcePad>,
	public Video::TVideoConsumerBase<Video::VideoConsumerPad>,
	public ZuazoBase
{
public:
	ShaderEffect(const std::string& fragShader, const std::string& uboName);
	ShaderEffect(const ShaderEffect& other)=delete;
	ShaderEffect(ShaderEffect&& other)=default;
	virtual ~ShaderEffect();

	template<typename T>
	void												setParam(const std::string& pname, const T& data);
	template<typename T>
	const T*											getParam(const std::string& pname) const;

	SUPPORTS_GETTING_PIXELFORMAT
	SUPPORTS_SETTING_PIXELFORMAT
	SUPPORTS_LISTING_PIXELFORMATS
	virtual std::set<Utils::PixelFormat>				getSupportedPixelFormats() const override;
	virtual void 										setPixelFormat(const Utils::PixelFormat& pixFmt) override;

	SUPPORTS_GETTING_RESOLUTION
	SUPPORTS_SETTING_RESOLUTION
	SUPPORTS_ANY_RESOLUTION
	virtual void										setResolution(const Utils::Resolution& res) override;

	void												open() override;
	void												close() override;

	void												update() const override;
private:
	std::string											m_fragShaderSrc;
	std::string											m_uboName;
	std::unique_ptr<Utils::Buffer>						m_data;

	std::unique_ptr<Graphics::Drawtable>				m_drawtable;
	std::unique_ptr<Graphics::ShaderUniform>			m_ubo;
	std::shared_ptr<Graphics::GL::Program>				m_shader;
	std::shared_ptr<Graphics::VertexArray>				m_vertexArray;

	static std::weak_ptr<Graphics::VertexArray>			s_vertexArray;
	static std::map<std::string, std::weak_ptr<Graphics::GL::Program>> s_shaders;
};

template<typename T>
void ShaderEffect::setParam(const std::string& pname, const T& data){
	if(m_shader){
		Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());

		GLint idx=m_shader->getUniformIndex(pname);
		size_t offset=m_shader->getUniformOffset(idx);
		size_t size=m_shader->getUniformSize(idx);

		if(sizeof(T) == size && m_ubo){
			m_ubo->setParam(data, offset);
		}
	}
}

template<typename T>
const T* ShaderEffect::getParam(const std::string& pname) const{
	if(m_shader){
		Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());

		GLint idx=m_shader->getUniformIndex(pname);
		size_t offset=m_shader->getUniformOffset(idx);
		size_t size=m_shader->getUniformSize(idx);

		if(sizeof(T) == size && m_data)
			return m_data->get() + offset;
	}
	return nullptr;
}
}
