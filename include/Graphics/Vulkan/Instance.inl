#include "Instance.h"

#include "../../Exception.h"

namespace Zuazo::Graphics::Vulkan {

template<typename F>
inline F Vulkan::Instance::getFunction(const std::string& name) const{
	auto func = vkGetInstanceProcAddr(m_instance, name.c_str());

	if(!func){
		throw Exception("Requested function " + name + " was not found");
	}

	return reinterpret_cast<F>(func);
}


}