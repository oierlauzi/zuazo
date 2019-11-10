#include <Graphics/Instance.h>

#include <Exception.h>

#include <cstring>

namespace Zuazo::Graphics {

std::function<uint32_t(const Vulkan::PhysicalDevice)> Instance::deviceScoreFunc;

Instance::Instance() :
	m_windowInstance(),
	m_vulkanInstance(getRequiredExtensions(), getRequieredValidationLayers()),
	m_messenger(getApplicationInfo().isDebug ? std::optional<Vulkan::Messenger>{m_vulkanInstance} : std::optional<Vulkan::Messenger>{}),
	m_physicalDevice(getBestPhysicalDevice(m_vulkanInstance.getPhysicalDevices())),
	m_device(m_physicalDevice, getRequieredQueueFamilies(m_physicalDevice)),
	m_graphicsQueue(m_device.getQueues()[0][0]),
	m_computeQueue(m_device.getQueues()[1][0]),
	m_tranferQueue(m_device.getQueues()[2][0]),
	m_presentationQueue(m_device.getQueues()[3][0])
{
}


Vulkan::Instance& Instance::getVulkanInstance(){
	return m_vulkanInstance;
}

Vulkan::Messenger& Instance::getMessenger(){
	return *m_messenger;
}

Vulkan::PhysicalDevice&	Instance::getPhysicalDevice(){
	return m_physicalDevice;
}

Vulkan::Device&	Instance::getDevice(){
	return m_device;
}




std::vector<Vulkan::Extension> Instance::getRequiredExtensions(){
	std::vector<Vulkan::Extension> extensions;

	//Add window swap-chain extensions
	const auto windowExtensions = Window::getRequiredVulkanExtensions();
	std::copy(
		windowExtensions.cbegin(),
		windowExtensions.cend(),
		std::back_insert_iterator(extensions)
	);

	if(Zuazo::getApplicationInfo().isDebug){
		//Add debug utils extension requirement
		Vulkan::Extension ext = {};
		std::strncpy(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
		extensions.push_back(ext);
	}

	return extensions;
}

std::vector<Vulkan::ValidationLayer> Instance::getRequieredValidationLayers(){
	std::vector<Vulkan::ValidationLayer> validationLayers;

	if(Zuazo::getApplicationInfo().isDebug){
		//Add debug utils extension requirement
		//Khronos validation layer
		Vulkan::ValidationLayer khronosValidation = {};
		std::strncpy(khronosValidation.layerName, "VK_LAYER_KHRONOS_validation", VK_MAX_EXTENSION_NAME_SIZE);
		validationLayers.push_back(khronosValidation);
	}

	return validationLayers;
}

std::vector<Vulkan::QueueFamily> Instance::getRequieredQueueFamilies(const Vulkan::PhysicalDevice& dev){
	std::vector<Vulkan::QueueFamily> queueFamilies{
		//Queue family 				Qty.	Ignore	Ignore
		{ VK_QUEUE_GRAPHICS_BIT, 	1, 		0, 		{}		},
		{ VK_QUEUE_COMPUTE_BIT, 	1, 		0, 		{}		},
		{ VK_QUEUE_TRANSFER_BIT, 	1, 		0, 		{}		},
	};

	queueFamilies.push_back(dev.getQueueFamilies()[dev.getPresentationIndices()[0]]); //Push the presentation queue compatible family

	return queueFamilies;
}




Vulkan::PhysicalDevice Instance::getBestPhysicalDevice(const std::vector<Vulkan::PhysicalDevice>& devices){
	std::pair<std::optional<Vulkan::PhysicalDevice>, uint32_t> best({}, 0);

	for(const auto& d: devices){
		Vulkan::PhysicalDevice devInfo(d);

		if(getPhysicalDeviceSupport(devInfo) == false)
			continue; //Not supported

		const uint32_t score = getPhysicalDeviceScore(devInfo);

		if(score > best.second){
			best = {devInfo, score};
		}
	}

	if(best.first.has_value() == false){
		throw Exception("No compatible GPU cards were found");
	}

	return best.first.value();
}

uint32_t Instance::getPhysicalDeviceScore(const Vulkan::PhysicalDevice& dev){
	return deviceScoreFunc ? deviceScoreFunc(dev) : defaultDeviceScoreFunc(dev);
}

bool Instance::getPhysicalDeviceSupport(const Vulkan::PhysicalDevice& dev){
	if(dev.getPresentationIndices().size() == 0){
		return false;
	}

	return true;
}

int32_t Instance::defaultDeviceScoreFunc(const Vulkan::PhysicalDevice& dev){
	int32_t score = 0;

	if(dev.getProperties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
		score += 1024;
	}

	score += dev.getProperties().limits.maxImageDimension2D;

	return score;
}

}