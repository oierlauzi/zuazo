#include "Vulkan.h"

namespace Zuazo::Graphics {

template<typename F>
inline F Vulkan::Instance::getFunction(const std::string& name) const{
	auto func = vkGetInstanceProcAddr(m_instance, name.c_str());

	if(!func){
		//TODO error
	}

	return reinterpret_cast<F>(func);
}

}