#include <Graphics/ShaderPool.h>

#include <Graphics/Context.h>

using namespace Zuazo::Graphics;

ShaderPool::~ShaderPool(){
	m_shaders.clear();
}

const GL::Shader& ShaderPool::get(const std::string& vert, const std::string& frag) const{
	ShaderSource key(vert, frag);

	const auto ite=m_shaders.find(key);

	if(ite == m_shaders.end()){
		m_shaders.emplace(std::piecewise_construct,
		          std::forward_as_tuple(key),
		          std::forward_as_tuple(vert, frag)
		);

		GL::Shader& shader=m_shaders[key];
		return shader;
	} else{
		return ite->second;
	}
}
