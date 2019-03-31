#pragma once

#include "GL/Shader.h"

#include <memory>
#include <map>

namespace Zuazo::Graphics{

class ShaderPool{
public:
	ShaderPool()=default;
	ShaderPool(const ShaderPool& other)=delete;
	ShaderPool(ShaderPool&& other)=default;
	~ShaderPool();

	const GL::Shader& get(const std::string& vert, const std::string& frag) const;
	void clear();
private:
	typedef std::pair<std::string, std::string> ShaderSource;
	mutable std::map<ShaderSource, GL::Shader>	m_shaders;
};

inline void ShaderPool::clear(){
	m_shaders.clear();
}

}
