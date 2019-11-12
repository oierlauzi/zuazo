#include <Graphics/Vulkan/Surface.h>

namespace Zuazo::Graphics::Vulkan {
	
Surface::Surface(Instance& instance , VkSurfaceKHR surface) :
	m_instance(instance),
	m_surface(surface)
{
}

Surface::~Surface(){
	if(m_surface){
		vkDestroySurfaceKHR(m_instance.get(), m_surface, nullptr);
	}
}


VkSurfaceKHR Surface::get(){
	return m_surface;
}

}