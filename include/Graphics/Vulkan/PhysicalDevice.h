#pragma once 

#include "Instance.h"

namespace Zuazo::Graphics::Vulkan {

class Instance;

class PhysicalDevice {
	friend Instance;
public:
	using QueueFamily = VkQueueFamilyProperties;

	PhysicalDevice(const PhysicalDevice& other) = default;
	~PhysicalDevice() = default;

	PhysicalDevice&						operator=(const PhysicalDevice& other) = default;

	VkPhysicalDevice					get();
	const VkPhysicalDeviceProperties&	getProperties() const;
	const VkPhysicalDeviceFeatures&		getFetures() const;
	const std::vector<QueueFamily>&		getQueueFamilies() const;
	const std::vector<uint32_t>&		getPresentationIndices() const;
private:
	PhysicalDevice(Instance& inst, VkPhysicalDevice dev);

	VkPhysicalDevice					m_device = nullptr;
	VkPhysicalDeviceProperties 			m_properties;
	VkPhysicalDeviceFeatures			m_features;
	std::vector<QueueFamily>			m_queueFamilies;
	std::vector<uint32_t>				m_presentationIndices;			

	static std::vector<QueueFamily>		getAvailableQueueFamilies(VkPhysicalDevice dev);
};

}