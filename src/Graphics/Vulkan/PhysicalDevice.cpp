#include <Graphics/Vulkan/PhysicalDevice.h>

#include <Graphics/Window.h>

namespace Zuazo::Graphics::Vulkan {


PhysicalDevice::PhysicalDevice(Instance& inst, VkPhysicalDevice dev) :
	m_device(dev),
	m_queueFamilies(getAvailableQueueFamilies(m_device))
{
	vkGetPhysicalDeviceProperties(m_device, &m_properties);
	vkGetPhysicalDeviceFeatures(m_device, &m_features);

	for(size_t i = 0; i < m_queueFamilies.size(); i++){
		if(Window::getPresentationSupport(inst , *this, i)){
			m_presentationIndices.push_back(i);
		}
	}
}

VkPhysicalDevice PhysicalDevice::get(){
	return m_device;
}

const VkPhysicalDeviceProperties& PhysicalDevice::getProperties() const{
	return m_properties;
}

const VkPhysicalDeviceFeatures& PhysicalDevice::getFetures() const{
	return m_features;
}

const std::vector<PhysicalDevice::QueueFamily>&	PhysicalDevice::getQueueFamilies() const{
	return m_queueFamilies;
}

const std::vector<uint32_t>& PhysicalDevice::getPresentationIndices() const{
	return m_presentationIndices;
}

std::vector<PhysicalDevice::QueueFamily> PhysicalDevice::getAvailableQueueFamilies(VkPhysicalDevice dev){
	//Get all the available validation layers
	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);

	std::vector<QueueFamily> queueFamilies(count);
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, queueFamilies.data());

	return queueFamilies;
}


}