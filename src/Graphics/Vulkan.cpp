#include <Graphics/Vulkan.h>

#include <Zuazo.h>
#include <Exception.h>
#include <Macros.h>
#include <Graphics/VulkanUtils.h>
#include <Graphics/Window.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <cassert>
#include <set>

namespace Zuazo::Graphics {


static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerCallback(
											VkDebugUtilsMessageSeverityFlagBitsEXT severity,
											VkDebugUtilsMessageTypeFlagsEXT type,
											const VkDebugUtilsMessengerCallbackDataEXT* data,
											void* userData ) 
{
	/*
	 * Based on:
	 * https://github.com/KhronosGroup/Vulkan-Hpp/blob/master/samples/CreateDebugUtilsMessenger/CreateDebugUtilsMessenger.cpp
	 */
	std::ostringstream message;

	message 
		<< vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)) 
		<< ": " << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(type)) << ":\n";
	message << "\t" << "messageIDName   = <" << data->pMessageIdName << ">\n";
	message << "\t" << "messageIdNumber = " << data->messageIdNumber << "\n";
	message << "\t" << "message         = <" << data->pMessage << ">\n";

	if (0 < data->queueLabelCount) {
		message << "\t" << "Queue Labels:\n";
		for (uint8_t i = 0; i < data->queueLabelCount; i++) {
			message << "\t\t" << "lableName = <" << data->pQueueLabels[i].pLabelName << ">\n";
		}
	}

	if (0 < data->cmdBufLabelCount) {
		message << "\t" << "CommandBuffer Labels:\n";
		for (uint8_t i = 0; i < data->cmdBufLabelCount; i++) {
			message << "\t\t" << "labelName = <" << data->pCmdBufLabels[i].pLabelName << ">\n";
		}
	}

	if (0 < data->objectCount) {
		message << "\t" << "Objects:\n";
		for (uint8_t i = 0; i < data->objectCount; i++) {
			message << "\t\t" << "Object " << i << "\n";
			message 
				<< "\t\t\t" << "objectType   = " 
				<< vk::to_string(static_cast<vk::ObjectType>(data->pObjects[i].objectType)) << "\n";
			message << "\t\t\t" << "objectHandle = " << data->pObjects[i].objectHandle << "\n";
			if (data->pObjects[i].pObjectName) {
				message << "\t\t\t" << "objectName   = <" << data->pObjects[i].pObjectName << ">\n";
			}
		}
	}

	std::cerr << message.str() << std::endl;

	ZUAZO_IGNORE_PARAM(userData);

	return VK_FALSE;
}




Vulkan::Vulkan(	const std::string& appName, 
		const Version& appVersion, 
		Verbosity verbosity, 
		const DeviceScoreFunc& scoreFunc )

	: m_instance(createInstance(appName, appVersion, verbosity))
	, m_loader(*m_instance, vk::Device())
	, m_messenger(createMessenger(*m_instance, m_loader, verbosity))
	, m_physicalDevice(getBestPhysicalDevice(*m_instance, scoreFunc))
	, m_queueIndices(getQueueIndices(*m_instance, m_physicalDevice))
	, m_device(createDevice(m_physicalDevice, m_queueIndices))
	, m_queues(getQueues(*m_device, m_queueIndices))
{
}

const vk::Instance& Vulkan::getInstance() const{
	return *m_instance;
}

const vk::PhysicalDevice& Vulkan::getPhysicalDevice() const{
	return m_physicalDevice;
}

const vk::Device& Vulkan::getDevice() const{
	return *m_device;
}

const vk::Queue& Vulkan::getGraphicsQueue() const{
	return m_queues[GRAPHICS_QUEUE];
}

uint32_t Vulkan::getGraphicsQueueIndex() const{
	return m_queueIndices[GRAPHICS_QUEUE];
}

const vk::Queue& Vulkan::getComputeQueue() const{
	return m_queues[COMPUTE_QUEUE];
}

uint32_t Vulkan::getComputeQueueIndex() const{
	return m_queueIndices[COMPUTE_QUEUE];
}

const vk::Queue& Vulkan::getTransferQueue() const{
	return m_queues[TRANSFER_QUEUE];
}

uint32_t Vulkan::getTransferQueueIndex() const{
	return m_queueIndices[TRANSFER_QUEUE];
}

const vk::Queue& Vulkan::getPresentationQueue() const{
	return m_queues[PRESENTATION_QUEUE];
}

uint32_t Vulkan::getPresentationQueueIndex() const{
	return m_queueIndices[PRESENTATION_QUEUE];
}



uint32_t Vulkan::defaultDeviceScoreFunc(const vk::PhysicalDevice& physicalDevice){
	int32_t score = 0;
	const auto properties = physicalDevice.getProperties();
	//const auto features = dev.getFeatures();

	if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu){
		score += 1024;
	}

	score += properties.limits.maxImageDimension2D;

	return score;
}



vk::UniqueInstance Vulkan::createInstance(	const std::string& appName, 
											const Version& appVersion, 
											Verbosity verbosity )
{
	//Set the application information
	vk::ApplicationInfo appInfo {
		appName.c_str(),
		VK_MAKE_VERSION(appVersion[0], appVersion[1], appVersion[2]),
		"Zuazo",
		VK_MAKE_VERSION(version[0], version[1], version[2]),
		VK_API_VERSION_1_0
	};


	//Get the validation layers
	auto requiredLayers = getRequiredLayers(verbosity);
	const auto availableLayers = vk::enumerateInstanceLayerProperties();
	const auto usedLayers = getUsedLayers(availableLayers, requiredLayers);

	if(requiredLayers.size()){
		//There are missing layers
		std::string missingNames;

		for(const auto& m : requiredLayers){
			missingNames += m.layerName;
			missingNames += "\n";
		}

		throw Exception("Missing Vulkan validation layers:\n" + missingNames);
	}

	const auto usedLayerNames = getNames(usedLayers);


	//Get the extensions
	auto requiredExtensions = getRequiredInstanceExtensions(verbosity);
	const auto availableExtensions = vk::enumerateInstanceExtensionProperties();
	const auto usedExtensions = getUsedExtensions(availableExtensions, requiredExtensions);

	if(requiredExtensions.size()){
		//There are missing extensions
		std::string missingNames;

		for(const auto& m : requiredExtensions){
			missingNames += m.extensionName;
			missingNames += "\n";
		}

		throw Exception("Missing Vulkan extensions:\n" + missingNames);
	}


	const auto usedExtensionNames = getNames(usedExtensions);


	//Create the vulkan instance
	vk::InstanceCreateInfo createInfo(
		{},
		&appInfo,
		usedLayerNames.size(),
		usedLayerNames.data(),
		usedExtensionNames.size(),
		usedExtensionNames.data()
	);

	return vk::createInstanceUnique(createInfo);
}

Vulkan::UniqueDebugUtilsMessengerEXT Vulkan::createMessenger(	const vk::Instance& instance, 
																const vk::DispatchLoaderDynamic& loader, 
																Verbosity verbosity ) 
{
	using Deleter = vk::UniqueHandleTraits<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>::deleter;

	vk::DebugUtilsMessengerEXT messenger;

	if(verbosity == Verbosity::ENABLE_VALIDATION_LAYERS){
		vk::DebugUtilsMessengerCreateInfoEXT createInfo(
			{},
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			validationLayerCallback,
			nullptr
		);

		messenger = instance.createDebugUtilsMessengerEXT(createInfo, nullptr, loader);
	}

	return UniqueDebugUtilsMessengerEXT(
		messenger,
		Deleter(instance, nullptr, loader)
	);
}

vk::PhysicalDevice Vulkan::getBestPhysicalDevice(	const vk::Instance& instance, 
													const DeviceScoreFunc& scoreFunc )
{
	const auto devices = instance.enumeratePhysicalDevices();

	std::pair<std::optional<vk::PhysicalDevice>, uint32_t> best({}, 0);

	for(const auto& device : devices){

		if(getPhysicalDeviceSupport(instance, device) == false){
			continue; //Not supported
		}

		const uint32_t score = scoreFunc(device);

		if(score > best.second){
			best = {device, score};
		}
	}

	if(best.first.has_value() == false){
		throw Exception("No compatible GPUs were found");
	}

	return best.first.value();
}

std::array<uint32_t, Vulkan::QUEUE_NUM>	Vulkan::getQueueIndices(const vk::Instance& inst, 
																const vk::PhysicalDevice& dev )
{
	std::array<uint32_t, Vulkan::QUEUE_NUM>	queues;

	const auto queueFamilies = dev.getQueueFamilyProperties();

	//Add the queue families
	queues[GRAPHICS_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eGraphics);
	queues[COMPUTE_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eCompute);
	queues[TRANSFER_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eTransfer);


	//Add a queue family compatible with presentation
	const auto presentFamilies = Window::getPresentationQueueFamilies(inst, dev);
	if(presentFamilies.size() == 0){
		throw Exception("Device has not presentation queues");
	}

	//Try to assign the graphics queue
	if(std::find(presentFamilies.cbegin(), presentFamilies.cend(), queues[GRAPHICS_QUEUE]) != presentFamilies.cend()){
		queues[PRESENTATION_QUEUE] = queues[GRAPHICS_QUEUE];
	} else {
		queues[PRESENTATION_QUEUE] = presentFamilies[0];
	}

	return queues;
}


vk::UniqueDevice Vulkan::createDevice(	const vk::PhysicalDevice& physicalDevice,
										const std::array<uint32_t, QUEUE_NUM>& queueIndices )
{
	//Get the queues
	std::set<uint32_t> uniqueQueueFamilies(queueIndices.cbegin(), queueIndices.cend());

	//Get the extensions
	auto requiredExtensions = getRequiredDeviceExtensions();
	const auto availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	const auto usedExtensions = getUsedExtensions(availableExtensions, requiredExtensions);

	if(requiredExtensions.size()){
		//There are missing extensions
		std::string missingNames;

		for(const auto& m : requiredExtensions){
			missingNames += m.extensionName;
			missingNames += "\n";
		}

		throw Exception("Missing Vulkan device extensions:\n" + missingNames);
	}


	const auto usedExtensionNames = getNames(usedExtensions);

	//Fill the queue create infos
	const float queuePriority = 1.0f;
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.reserve(uniqueQueueFamilies.size());

	for(auto q : uniqueQueueFamilies){
		queueCreateInfos.emplace_back(
			vk::DeviceQueueCreateFlags(),
			q,
			1,
			&queuePriority
		);
	}


	vk::DeviceCreateInfo createInfo(
		{},
		queueCreateInfos.size(),
		queueCreateInfos.data(),
		0,
		nullptr,
		usedExtensionNames.size(),
		usedExtensionNames.data()
	);

	return physicalDevice.createDeviceUnique(createInfo);
}

std::array<vk::Queue, Vulkan::QUEUE_NUM> Vulkan::getQueues(	const vk::Device& device, 
															const std::array<uint32_t, QUEUE_NUM>& queueIndices )
{
	std::array<vk::Queue, Vulkan::QUEUE_NUM> queues;

	for(size_t i = 0; i < QUEUE_NUM; i++){
		queues[i] = device.getQueue(queueIndices[i], 0);
	}

	return queues;
}




std::vector<vk::LayerProperties> Vulkan::getRequiredLayers(Verbosity verbosity){
	std::vector<vk::LayerProperties> validationLayers;

	if(verbosity == Verbosity::ENABLE_VALIDATION_LAYERS){
		//Add debug utils extension requirement
		//Khronos validation layer
		VkLayerProperties khronosValidation = {};
		std::strncpy(khronosValidation.layerName, "VK_LAYER_KHRONOS_validation", VK_MAX_EXTENSION_NAME_SIZE);
		validationLayers.emplace_back(khronosValidation);
	}

	return validationLayers;
}

std::vector<vk::ExtensionProperties> Vulkan::getRequiredInstanceExtensions(Verbosity verbosity){
	std::vector<vk::ExtensionProperties> extensions;

	//Add window swap-chain extensions
	const auto windowExtensions = Window::getRequiredVulkanExtensions();
	std::copy(
		windowExtensions.cbegin(),
		windowExtensions.cend(),
		std::back_insert_iterator(extensions)
	);

	if(verbosity == Verbosity::ENABLE_VALIDATION_LAYERS){
		//Add debug utils extension requirement
		VkExtensionProperties ext = {};
		std::strncpy(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
		extensions.emplace_back(ext);
	}

	return extensions;
}

std::vector<vk::ExtensionProperties> Vulkan::getRequiredDeviceExtensions(){
	std::vector<vk::ExtensionProperties> extensions;

	//Require swapchain extension
	VkExtensionProperties swapchainExtension = {};
	std::strncpy(swapchainExtension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
	extensions.emplace_back(swapchainExtension);

	return extensions;
}

std::vector<vk::QueueFamilyProperties> 	Vulkan::getRequiredQueueFamilies(){
	return {
		VkQueueFamilyProperties{ VK_QUEUE_GRAPHICS_BIT,	1,	0,	{} },
		VkQueueFamilyProperties{ VK_QUEUE_COMPUTE_BIT,	1,	0,	{} },
		VkQueueFamilyProperties{ VK_QUEUE_TRANSFER_BIT,	1,	0,	{} }
	};
}

bool Vulkan::getPhysicalDeviceSupport(const vk::Instance& instance, const vk::PhysicalDevice& device){
	//Window support is required
	if(Window::getPresentationQueueFamilies(instance, device).size() == 0){
		return false;
	}

	//Check device extension support
	auto requiredExtensions = getRequiredDeviceExtensions();
	const auto availableExtensions = device.enumerateDeviceExtensionProperties();
	getUsedExtensions(availableExtensions, requiredExtensions);
	if(requiredExtensions.size()){
		return false;
	}

	//Check device queue family support
	auto requiredQueueFamilies = getRequiredQueueFamilies();
	const auto availableQueueFamilies = device.getQueueFamilyProperties();
	getUsedQueueFamilies(availableQueueFamilies, requiredQueueFamilies);
	if(requiredQueueFamilies.size()){
		return false;
	}

	return true;
}

}