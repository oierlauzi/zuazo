#include <Graphics/Semaphore.h>

namespace Zuazo::Graphics {

Semaphore::Semaphore(const Vulkan& vulkan) 
	: m_semaphore(createSemaphore(vulkan))	
{
}

vk::Semaphore Semaphore::getSemaphore() const{
	return *m_semaphore;
}

vk::UniqueSemaphore Semaphore::createSemaphore(const Vulkan& vulkan){
	const vk::SemaphoreCreateInfo createInfo;
	return vulkan.getDevice().createSemaphoreUnique(createInfo, nullptr, vulkan.getDispatcher());
}
}