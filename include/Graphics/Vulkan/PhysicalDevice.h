#pragma once 

#include "Queue.h"
#include "Extension.h"
#include "ValidationLayer.h"

#include <vector>

#include <vulkan/vulkan.h>


namespace Zuazo::Graphics::Vulkan {

class Instance;

class PhysicalDevice {
	friend Instance;
public:
	using Properties = VkPhysicalDeviceProperties;
	using Features = VkPhysicalDeviceFeatures;

	~PhysicalDevice() = default;

	VkPhysicalDevice					get();

	const Properties&					getProperties() const;
	const Features&						getFeatures() const;
	const std::vector<QueueFamily>&		getQueueFamilies() const;
	const std::vector<Extension>&		getAvailableExtensions() const;
	const std::vector<ValidationLayer>&	getAvailableValidationLayers() const;

private:
	PhysicalDevice(VkPhysicalDevice dev);

	VkPhysicalDevice					m_physicalDevice = nullptr;

	Properties							m_properties;
	Features							m_features;
	std::vector<QueueFamily>			m_queueFamilies;
	std::vector<Extension>				m_availableExtensions;
	std::vector<ValidationLayer>		m_availableValidationLayers;

	static std::vector<QueueFamily>		getQueueFamilies(VkPhysicalDevice dev);
	static std::vector<Extension>		getAvailableExtensions(VkPhysicalDevice dev);
	static std::vector<ValidationLayer>	getAvailableValidationLayers(VkPhysicalDevice dev);
};

}