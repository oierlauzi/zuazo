#include <Processors/ShaderEffect.h>

#include <Graphics/Context.h>

using namespace Zuazo::Processors;

std::weak_ptr<Zuazo::Graphics::VertexArray> ShaderEffect::s_vertexArray;
std::map<std::string, std::weak_ptr<Zuazo::Graphics::GL::Program>> ShaderEffect::s_shaders;


ShaderEffect::ShaderEffect(const std::string& fragShader, const std::string& uboName) :
		m_fragShaderSrc(fragShader),
		m_uboName(uboName)
{
	open();
}

ShaderEffect::~ShaderEffect(){
	close();
}


std::set<Zuazo::Utils::PixelFormat> ShaderEffect::getSupportedPixelFormats() const{
	return std::set<Zuazo::Utils::PixelFormat>();
}

void ShaderEffect::setPixelFormat(const Utils::PixelFormat& pixFmt){

}

void ShaderEffect::setResolution(const Utils::Resolution& res){

}

void ShaderEffect::open(){
	Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());

}

void ShaderEffect::close(){
	Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());
	m_drawtable.reset();
	m_ubo.reset();
	m_shader.reset();
	m_vertexArray.reset();

	//I was the only one with this shader. Delete it
	auto ite=s_shaders.find(m_fragShaderSrc);
	if(ite != s_shaders.end()){
		if(ite->second.expired()){
			s_shaders.erase(ite);
		}
	}

}

void ShaderEffect::update() const{

}
