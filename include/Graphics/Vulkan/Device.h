#pragma once 

#include "Instance.h"
#include "PhysicalDevice.h"

#include <vector>
#include <map>

namespace Zuazo::Graphics::Vulkan {

using QueueFamily = VkQueueFamilyProperties;
using Queue = VkQueue;

class Device {
public:	
	Device(PhysicalDevice& dev, const std::vector<QueueFamily>& queueFamilies);
	Device(const Device& other) = delete;
	~Device();

	VkDevice								get();
	const std::vector<std::vector<Queue>>& 	getQueues();

private:
	VkDevice								m_device = nullptr;
	std::vector<std::vector<Queue>>			m_queues;

	static std::vector<QueueFamily>			getQueueFamilies(const std::vector<QueueFamily>& available, std::vector<QueueFamily> required);
	static std::map<uint32_t, uint32_t>		getQueueFamilyIndices(const std::vector<QueueFamily>& available, std::vector<QueueFamily> requested);
	static size_t							getQueueFamilyIndex(const std::vector<QueueFamily>& qf, uint32_t flags);
	static bool								isSuitable(uint32_t required, uint32_t available);
};

}