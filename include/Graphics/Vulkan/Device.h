#pragma once 

#include "Queue.h"
#include "PhysicalDevice.h"
#include "Extension.h"
#include "ValidationLayer.h"

#include <vector>
#include <map>

namespace Zuazo::Graphics::Vulkan {

class Device {
public:	
	Device(PhysicalDevice& dev, const std::vector<QueueFamily>& queueFamilies, const std::vector<Extension>& extensions, const std::vector<ValidationLayer>& validationLayers);
	Device(const Device& other) = delete;
	~Device();

	VkDevice								get();

	const std::vector<std::vector<Queue>>&	getQueues() const;
private:
	PhysicalDevice& 						m_physicalDevice;

	VkDevice								m_device = nullptr;

	std::vector<std::vector<Queue>>			m_queues;
};

}