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

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

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

	: m_loader()
	, m_dispatcher(createDispatcher(m_loader))
	, m_instance(createInstance(m_dispatcher, appName, appVersion, verbosity))
	, m_messenger(createMessenger(m_dispatcher, *m_instance, verbosity))
	, m_physicalDevice(getBestPhysicalDevice(m_dispatcher, *m_instance, scoreFunc))
	, m_queueIndices(getQueueIndices(m_dispatcher, *m_instance, m_physicalDevice))
	, m_device(createDevice(m_dispatcher, m_physicalDevice, m_queueIndices))
	, m_queues(getQueues(m_dispatcher, *m_device, m_queueIndices))
{
}

const vk::DynamicLoader& Vulkan::getLoader() const{
	return m_loader;
}

const vk::DispatchLoaderDynamic& Vulkan::getDispatcher() const{
	return m_dispatcher;
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

uint32_t Vulkan::defaultDeviceScoreFunc(const vk::DispatchLoaderDynamic& disp, const vk::PhysicalDevice& physicalDevice){
	int32_t score = 0;
	const auto properties = physicalDevice.getProperties(disp);
	//const auto features = dev.getFeatures();

	if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu){
		score += 1024;
	}

	score += properties.limits.maxImageDimension2D;

	return score;
}



vk::DispatchLoaderDynamic Vulkan::createDispatcher(const vk::DynamicLoader& loader){
	vk::DispatchLoaderDynamic dispatch;

	auto vkGetInstanceProcAddr = loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	dispatch.init(vkGetInstanceProcAddr);

	return dispatch;
}

vk::UniqueInstance Vulkan::createInstance(	vk::DispatchLoaderDynamic& disp,
											const std::string& appName, 
											const Version& appVersion, 
											Verbosity verbosity )
{
	//Set the application information
	vk::ApplicationInfo appInfo(
		appName.c_str(),
		VK_MAKE_VERSION(appVersion[0], appVersion[1], appVersion[2]),
		"Zuazo",
		VK_MAKE_VERSION(version[0], version[1], version[2]),
		VK_API_VERSION_1_1
	);

	//Get the validation layers
	auto requiredLayers = getRequiredLayers(verbosity);
	const auto availableLayers = vk::enumerateInstanceLayerProperties(disp);
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
	const auto availableExtensions = vk::enumerateInstanceExtensionProperties(nullptr, disp);
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
	auto instance = vk::createInstanceUnique(createInfo, nullptr, disp);

	//Add the available functions to the dispatcher
	disp.init(*instance);

	return instance;
}

vk::UniqueDebugUtilsMessengerEXT Vulkan::createMessenger(	const vk::DispatchLoaderDynamic& disp,
															const vk::Instance& instance,
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

		messenger = instance.createDebugUtilsMessengerEXT(createInfo, nullptr, disp);
	}

	return vk::UniqueDebugUtilsMessengerEXT(
		messenger,
		Deleter(instance, nullptr, disp)
	);
}

vk::PhysicalDevice Vulkan::getBestPhysicalDevice(	const vk::DispatchLoaderDynamic& disp,
													const vk::Instance& instance, 
													const DeviceScoreFunc& scoreFunc )
{
	const auto devices = instance.enumeratePhysicalDevices(disp);

	std::pair<std::optional<vk::PhysicalDevice>, uint32_t> best({}, 0);

	for(const auto& device : devices){

		if(getPhysicalDeviceSupport(disp, instance, device) == false){
			continue; //Not supported
		}

		const uint32_t score = scoreFunc(disp, device);

		if(score > best.second){
			best = {device, score};
		}
	}

	if(best.first.has_value() == false){
		throw Exception("No compatible GPUs were found");
	}

	return best.first.value();
}

std::array<uint32_t, Vulkan::QUEUE_NUM>	Vulkan::getQueueIndices(const vk::DispatchLoaderDynamic& disp,
																const vk::Instance& inst, 
																const vk::PhysicalDevice& dev )
{
	std::array<uint32_t, Vulkan::QUEUE_NUM>	queues;

	const auto queueFamilies = dev.getQueueFamilyProperties(disp);

	//Add the queue families
	queues[GRAPHICS_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eGraphics);
	queues[COMPUTE_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eCompute);
	queues[TRANSFER_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eTransfer);


	//Add a queue family compatible with presentation
	const auto presentFamilies = Window::getPresentationQueueFamilies(disp, inst, dev);
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


vk::UniqueDevice Vulkan::createDevice(	vk::DispatchLoaderDynamic& disp,
										const vk::PhysicalDevice& physicalDevice,
										const std::array<uint32_t, QUEUE_NUM>& queueIndices )
{
	//Get the queues
	std::set<uint32_t> uniqueQueueFamilies(queueIndices.cbegin(), queueIndices.cend());

	//Get the extensions
	auto requiredExtensions = getRequiredDeviceExtensions();
	const auto availableExtensions = physicalDevice.enumerateDeviceExtensionProperties(nullptr, disp);
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
	auto dev = physicalDevice.createDeviceUnique(createInfo, nullptr, disp);

	//Register the device specific functions 
	disp.init(*dev);

	return dev;
}

std::array<vk::Queue, Vulkan::QUEUE_NUM> Vulkan::getQueues(	const vk::DispatchLoaderDynamic& disp,
															const vk::Device& device, 
															const std::array<uint32_t, QUEUE_NUM>& queueIndices )
{
	std::array<vk::Queue, Vulkan::QUEUE_NUM> queues;

	for(size_t i = 0; i < QUEUE_NUM; i++){
		queues[i] = device.getQueue(queueIndices[i], 0, disp);
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

std::vector<vk::QueueFamilyProperties> Vulkan::getRequiredQueueFamilies(){
	return {
		VkQueueFamilyProperties{ VK_QUEUE_GRAPHICS_BIT,	1,	0,	{} },
		VkQueueFamilyProperties{ VK_QUEUE_COMPUTE_BIT,	1,	0,	{} },
		VkQueueFamilyProperties{ VK_QUEUE_TRANSFER_BIT,	1,	0,	{} }
	};
}

bool Vulkan::getPhysicalDeviceSupport(	const vk::DispatchLoaderDynamic& disp,
										const vk::Instance& instance, 
										const vk::PhysicalDevice& device )
{	
	auto features = device.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceSamplerYcbcrConversionFeatures>(disp);

	//Require YCbCr support
	const auto& ycbcrSamplerSupport = features.get<vk::PhysicalDeviceSamplerYcbcrConversionFeatures>();

	if(ycbcrSamplerSupport.samplerYcbcrConversion == false){
		return false;
	}


	//Window support is required
	if(Window::getPresentationQueueFamilies(disp, instance, device).size() == 0){
		return false;
	}

	//Check device extension support
	auto requiredExtensions = getRequiredDeviceExtensions();
	const auto availableExtensions = device.enumerateDeviceExtensionProperties(nullptr, disp);
	getUsedExtensions(availableExtensions, requiredExtensions);
	if(requiredExtensions.size()){
		return false;
	}

	//Check device queue family support
	auto requiredQueueFamilies = getRequiredQueueFamilies();
	const auto availableQueueFamilies = device.getQueueFamilyProperties(disp);
	getUsedQueueFamilies(availableQueueFamilies, requiredQueueFamilies);
	if(requiredQueueFamilies.size()){
		return false;
	}

	return true;
}

}