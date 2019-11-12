#include <Graphics/Instance.h>

#include <Exception.h>
#include <Macros.h>

#include <cstring>
#include <iostream>

namespace Zuazo::Graphics {


static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerCallback(
											VkDebugUtilsMessageSeverityFlagBitsEXT severity,
											VkDebugUtilsMessageTypeFlagsEXT type,
											const VkDebugUtilsMessengerCallbackDataEXT* data,
											void* userData ) 
{
	ZUAZO_IGNORE_PARAM(severity);
	ZUAZO_IGNORE_PARAM(type);
	ZUAZO_IGNORE_PARAM(userData);

	std::cerr << "Vulkan validation layer: " << data->pMessage << std::endl;
	return VK_FALSE;
}



Instance::DeviceScoreFunc Instance::deviceScoreFunc;

Instance::Instance() :
	m_windowInstance(),
	m_vulkanInstance(getRequiredInstanceExtensions(), getRequiredValidationLayers()),
	m_messenger(getMessenger(m_vulkanInstance)),
	m_physicalDevice(getBestPhysicalDevice(m_vulkanInstance)),
	m_device(m_physicalDevice, getRequiredQueueFamilies(m_vulkanInstance, m_physicalDevice), getRequiredDeviceExtensions(), getRequiredValidationLayers()),
	m_queues(m_device.getQueues())
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

Vulkan::Queue Instance::getGraphicsQueue(){
	return m_queues[GRAPHICS_QUEUE][0];
}

Vulkan::Queue Instance::getComputeQueue(){
	return m_queues[COMPUTE_QUEUE][0];
}

Vulkan::Queue Instance::getTranferQueue(){
	return m_queues[TRANSFER_QUEUE][0];
}

Vulkan::Queue Instance::getPresentationQueue(){
	return m_queues[PRESENTATION_QUEUE][0];
}







std::vector<Vulkan::ValidationLayer> Instance::getRequiredValidationLayers(){
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

std::vector<Vulkan::Extension> Instance::getRequiredInstanceExtensions(){
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

std::optional<Vulkan::Messenger> Instance::getMessenger(Vulkan::Instance& inst){
	if(getApplicationInfo().isDebug){
		//Create it
		return std::optional<Vulkan::Messenger>(std::in_place, inst, validationLayerCallback);
	} else {
		//No need to create it
		return {};
	}
}

std::vector<Vulkan::Extension> Instance::getRequiredDeviceExtensions(){
	std::vector<Vulkan::Extension> extensions;

	//Require swapchain extension
	Vulkan::Extension swapchainExtension = {};
	std::strncpy(swapchainExtension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
	extensions.push_back(swapchainExtension);


	return extensions;
}

std::vector<Vulkan::QueueFamily> Instance::getRequiredQueueFamilies(Vulkan::Instance& inst, Vulkan::PhysicalDevice& dev){
	std::vector<Vulkan::QueueFamily> queueFamilies(QUEUE_COUNT);

	//								  Queue family 				Qty.	Ignore	Ignore
	queueFamilies[GRAPHICS_QUEUE] = { VK_QUEUE_GRAPHICS_BIT, 	1, 		0, 		{}		};
	queueFamilies[COMPUTE_QUEUE]  = { VK_QUEUE_COMPUTE_BIT, 	1, 		0, 		{}		};
	queueFamilies[TRANSFER_QUEUE] = { VK_QUEUE_TRANSFER_BIT, 	1, 		0, 		{}		};
	
	const size_t presentationQueueIndex = Window::getPresentationQueueFamilies(inst, dev)[0];
	queueFamilies[PRESENTATION_QUEUE] = dev.getQueueFamilies()[presentationQueueIndex];

	return queueFamilies;
}




Vulkan::PhysicalDevice Instance::getBestPhysicalDevice(Vulkan::Instance& inst){
	const auto devices = inst.getPhysicalDevices();

	std::pair<std::optional<Vulkan::PhysicalDevice>, uint32_t> best({}, 0);

	for(const auto& d: devices){
		Vulkan::PhysicalDevice devInfo(d);

		if(getPhysicalDeviceSupport(inst, devInfo) == false)
			continue; //Not supported

		const uint32_t score = getPhysicalDeviceScore(devInfo);

		if(score > best.second){
			best = {devInfo, score};
		}
	}

	if(best.first.has_value() == false){
		throw Exception("No compatible GPUs were found");
	}

	return best.first.value();
}

uint32_t Instance::getPhysicalDeviceScore(const Vulkan::PhysicalDevice& dev){
	return deviceScoreFunc ? deviceScoreFunc(dev) : defaultDeviceScoreFunc(dev);
}

bool Instance::getPhysicalDeviceSupport(Vulkan::Instance& inst, Vulkan::PhysicalDevice& dev){
	//Window support is required
	if(Window::getPresentationQueueFamilies(inst, dev).size() == 0){
		return false;
	}

	//Check device queue family support
	try {
		const auto& availableQueueFamilies = dev.getQueueFamilies();
		const auto requiredQueueFamilies = getRequiredQueueFamilies(inst, dev);

		//This should thow if they are incompatible
		Vulkan::getUsedQueueFamilies(availableQueueFamilies, requiredQueueFamilies);
	} catch (Exception& e){
		return false;
	}

	//Check device extension support
	try {
		const auto& availableExtensions = dev.getAvailableExtensions();
		const auto requiredExtensions = getRequiredDeviceExtensions();

		//This should thow if they are incompatible
		Vulkan::getUsedExtensions(availableExtensions, requiredExtensions);
	} catch (Exception& e){
		return false;
	}

	//Check device validation layer support
	try {
		const auto& availableValidationLayers = dev.getAvailableValidationLayers();
		const auto requiredValidationLayers = getRequiredValidationLayers();

		//This should thow if they are incompatible
		Vulkan::getUsedValidationLayers(availableValidationLayers, requiredValidationLayers);
	} catch (Exception& e){
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