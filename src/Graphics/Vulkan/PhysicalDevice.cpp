#include <Graphics/Vulkan/PhysicalDevice.h>

namespace Zuazo::Graphics::Vulkan {


PhysicalDevice::PhysicalDevice(VkPhysicalDevice dev) :
	m_physicalDevice(dev),
	m_queueFamilies(getQueueFamilies(m_physicalDevice)),
	m_availableExtensions(getAvailableExtensions(m_physicalDevice)),
	m_availableValidationLayers(getAvailableValidationLayers(m_physicalDevice))
{
	//Query features and properties
	vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);
	vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_features);
}

VkPhysicalDevice PhysicalDevice::get(){
	return m_physicalDevice;
}

const PhysicalDevice::Properties& PhysicalDevice::getProperties() const{
	return m_properties;
}

const PhysicalDevice::Features& PhysicalDevice::getFeatures() const{
	return m_features;
}

const std::vector<QueueFamily>& PhysicalDevice::getQueueFamilies() const{
	return m_queueFamilies;
}

const std::vector<Extension>& PhysicalDevice::getAvailableExtensions() const{
	return m_availableExtensions;
}

const std::vector<ValidationLayer>&	PhysicalDevice::getAvailableValidationLayers() const{
	return m_availableValidationLayers;
}




std::vector<QueueFamily> PhysicalDevice::getQueueFamilies(VkPhysicalDevice dev) {
	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
	std::vector<QueueFamily> queueFamilies(count);
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, queueFamilies.data());

	return queueFamilies;
}

std::vector<Extension> PhysicalDevice::getAvailableExtensions(VkPhysicalDevice dev){
	uint32_t count;
	vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, nullptr);
	std::vector<Extension> extensions(count);
	vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, extensions.data());

	return extensions;
}

std::vector<ValidationLayer> PhysicalDevice::getAvailableValidationLayers(VkPhysicalDevice dev){
	uint32_t count;
	vkEnumerateDeviceLayerProperties(dev, &count, nullptr);
	std::vector<ValidationLayer> validationLayers(count);
	vkEnumerateDeviceLayerProperties(dev, &count, validationLayers.data());

	return validationLayers;
}

}