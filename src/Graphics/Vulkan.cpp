#include <Graphics/Vulkan.h>

#include <Zuazo.h>
#include <Exception.h>
#include <Macros.h>
#include <Graphics/Window.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <cassert>

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

	message << "On Vulkan: " << userData << "\n";
	message << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)) << ": " << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(type)) << ":\n";
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
			message << "\t\t\t" << "objectType   = " << vk::to_string(static_cast<vk::ObjectType>(data->pObjects[i].objectType)) << "\n";
			message << "\t\t\t" << "objectHandle = " << data->pObjects[i].objectHandle << "\n";
			if (data->pObjects[i].pObjectName) {
				message << "\t\t\t" << "objectName   = <" << data->pObjects[i].pObjectName << ">\n";
			}
		}
	}

	std::cerr << message.str() << std::endl;

	return VK_FALSE;
}







Vulkan::DeviceScoreFunc Vulkan::deviceScoreFunc = [] (const vk::PhysicalDevice& dev) -> uint32_t {
	int32_t score = 0;
	const auto properties = dev.getProperties();
	//const auto features = dev.getFeatures();

	if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu){
		score += 1024;
	}

	score += properties.limits.maxImageDimension2D;

	return score;
};






Vulkan::Vulkan() 
	: m_instance(createInstance())
	, m_loader(*m_instance, vk::Device())
	, m_messenger(createMessenger(*m_instance, m_loader, this))
	, m_physicalDevice(getBestPhysicalDevice(*m_instance))
	, m_device(createDevice(*m_instance, m_physicalDevice))
	, m_queues(getQueues(*m_instance, m_physicalDevice, *m_device))
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

const vk::Queue& Vulkan::getComputeQueue() const{
	return m_queues[COMPUTE_QUEUE];
}

const vk::Queue& Vulkan::getTransferQueue() const{
	return m_queues[TRANSFER_QUEUE];
}

const vk::Queue& Vulkan::getPresentationQueue() const{
	return m_queues[PRESENTATION_QUEUE];
}




vk::UniqueInstance Vulkan::createInstance(){
	//Get the application information
	const auto& appName = Zuazo::getApplicationInfo().name;
	const auto& appVersion = Zuazo::getApplicationInfo().version;

	vk::ApplicationInfo appInfo {
		appName.c_str(),
		VK_MAKE_VERSION(appVersion[0], appVersion[1], appVersion[2]),
		"Zuazo",
		VK_MAKE_VERSION(version[0], version[1], version[2]),
		VK_API_VERSION_1_0
	};


	//Get the validation layers
	auto requiredLayers = getRequiredLayers();
	const auto availableLayers = getAvailableInstanceLayers();
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
	auto requiredExtensions = getRequiredInstanceExtensions();
	const auto availableExtensions = getAvailableInstanceExtensions();
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
																Vulkan* vk ) 
{
	using Deleter = vk::UniqueHandleTraits<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>::deleter;

	vk::DebugUtilsMessengerEXT messenger;

	if(getApplicationInfo().isDebug){
		vk::DebugUtilsMessengerCreateInfoEXT createInfo(
			{},
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			validationLayerCallback,
			vk
		);

		messenger = instance.createDebugUtilsMessengerEXT(createInfo, nullptr, loader);
	}

	return UniqueDebugUtilsMessengerEXT(
		messenger,
		Deleter(instance, nullptr, loader)
	);
}

vk::PhysicalDevice Vulkan::getBestPhysicalDevice(const vk::Instance& instance){
	const auto devices = instance.enumeratePhysicalDevices();

	std::pair<std::optional<vk::PhysicalDevice>, uint32_t> best({}, 0);

	for(const auto& device : devices){

		if(getPhysicalDeviceSupport(instance, device) == false){
			continue; //Not supported
		}

		const uint32_t score = deviceScoreFunc(device);

		if(score > best.second){
			best = {device, score};
		}
	}

	if(best.first.has_value() == false){
		throw Exception("No compatible GPUs were found");
	}

	return best.first.value();
}

vk::UniqueDevice Vulkan::createDevice(const vk::Instance& instance, const vk::PhysicalDevice& physicalDevice){
	//Get all the required queue families
	auto requiredQueueFamilies = getRequiredQueueFamilies(instance, physicalDevice);
	const auto availableQueueFamilies = physicalDevice.getQueueFamilyProperties();
	const auto usedQueueFamilies = getUsedQueueFamilies(availableQueueFamilies, requiredQueueFamilies);

	if(requiredQueueFamilies.size()){
		throw Exception("There are missing queue families");
	}

	const auto usedQueueFamilyIndices = getQueueFamilyIndices(availableQueueFamilies, usedQueueFamilies);


	//Get the validation layers
	auto requiredLayers = getRequiredLayers();
	const auto availableLayers = physicalDevice.enumerateDeviceLayerProperties();
	const auto usedLayers = getUsedLayers(availableLayers, requiredLayers);

	if(requiredLayers.size()){
		//There are missing layers
		std::string missingNames;

		for(const auto& m : requiredLayers){
			missingNames += m.layerName;
			missingNames += "\n";
		}

		throw Exception("Missing Vulkan device validation layers:\n" + missingNames);
	}

	const auto usedLayerNames = getNames(usedLayers);


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
	queueCreateInfos.reserve(usedQueueFamilyIndices.size());

	for(auto ite = usedQueueFamilyIndices.cbegin(); ite != usedQueueFamilyIndices.cend(); ++ite){
		queueCreateInfos.emplace_back(
			vk::DeviceQueueCreateFlags(),
			ite->first,
			ite->second,
			&queuePriority
		);
	}


	vk::DeviceCreateInfo createInfo(
		{},
		queueCreateInfos.size(),
		queueCreateInfos.data(),
		usedLayerNames.size(),
		usedLayerNames.data(),
		usedExtensionNames.size(),
		usedExtensionNames.data()
	);

	return physicalDevice.createDeviceUnique(createInfo);
}

std::array<vk::Queue, Vulkan::QUEUE_NUM> Vulkan::getQueues(	const vk::Instance& instance, 
															const vk::PhysicalDevice& physicalDevice, 
															const vk::Device& dev )
{
	const auto queues = getQueues(physicalDevice, dev, getRequiredQueueFamilies(instance, physicalDevice));

	std::array<vk::Queue, Vulkan::QUEUE_NUM> result;
	for(size_t i = 0; i < QUEUE_NUM; i++){
		result[i] = queues[i][0];
	}

	return result;
}





std::vector<vk::LayerProperties> Vulkan::getRequiredLayers(){
	std::vector<vk::LayerProperties> validationLayers;

	if(Zuazo::getApplicationInfo().isDebug){
		//Add debug utils extension requirement
		//Khronos validation layer
		VkLayerProperties khronosValidation = {};
		std::strncpy(khronosValidation.layerName, "VK_LAYER_KHRONOS_validation", VK_MAX_EXTENSION_NAME_SIZE);
		validationLayers.emplace_back(khronosValidation);
	}

	return validationLayers;
}

std::vector<vk::ExtensionProperties> Vulkan::getRequiredInstanceExtensions(){
	std::vector<vk::ExtensionProperties> extensions;

	//Add window swap-chain extensions
	const auto windowExtensions = Window::getRequiredVulkanExtensions();
	std::copy(
		windowExtensions.cbegin(),
		windowExtensions.cend(),
		std::back_insert_iterator(extensions)
	);

	if(Zuazo::getApplicationInfo().isDebug){
		//Add debug utils extension requirement
		VkExtensionProperties ext = {};
		std::strncpy(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
		extensions.emplace_back(ext);
	}

	return extensions;
}

std::vector<vk::QueueFamilyProperties> Vulkan::getRequiredQueueFamilies(const vk::Instance& inst, const vk::PhysicalDevice& dev){
	std::vector<vk::QueueFamilyProperties> queueFamilies(QUEUE_NUM);

	//								  Queue family 				Qty.	Ignore	Ignore
	queueFamilies[GRAPHICS_QUEUE] = { VK_QUEUE_GRAPHICS_BIT, 	1, 		0, 		{}		};
	queueFamilies[COMPUTE_QUEUE]  = { VK_QUEUE_COMPUTE_BIT, 	1, 		0, 		{}		};
	queueFamilies[TRANSFER_QUEUE] = { VK_QUEUE_TRANSFER_BIT, 	1, 		0, 		{}		};
	
	//Add a queue family compatible with presentation
	const auto presentationFamilies = Window::getPresentationQueueFamilies(inst, dev);
	if(presentationFamilies.size() == 0){
		throw Exception("Device has not presentation queues");
	}

	auto presentationQueue = dev.getQueueFamilyProperties()[presentationFamilies[0]];
	presentationQueue.queueCount = 1;
	queueFamilies[PRESENTATION_QUEUE] = presentationQueue;

	return queueFamilies;
}

std::vector<vk::ExtensionProperties> Vulkan::getRequiredDeviceExtensions(){
	std::vector<vk::ExtensionProperties> extensions;

	//Require swapchain extension
	VkExtensionProperties swapchainExtension = {};
	std::strncpy(swapchainExtension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
	extensions.emplace_back(swapchainExtension);

	return extensions;
}




std::vector<vk::LayerProperties> Vulkan::getAvailableInstanceLayers(){
	return vk::enumerateInstanceLayerProperties();
}

std::vector<vk::LayerProperties> Vulkan::getUsedLayers(	const std::vector<vk::LayerProperties>& available, 
														std::vector<vk::LayerProperties>& required )
{
	std::vector<vk::LayerProperties> layers;

	auto ite = required.begin();
	while(ite != required.end()){
		bool found = false;

		for(const auto& availableVl : available){
			if(std::strncmp(ite->layerName, availableVl.layerName, VK_MAX_EXTENSION_NAME_SIZE) == 0){
				if(ite->specVersion <= availableVl.specVersion){
					layers.push_back(availableVl);
					found = true;
					break;
				}
			}
		}

		//Advance to the next one
		if(found){
			ite = required.erase(ite);
		}else {
			++ite;
		}
	}

	return layers;
}

std::vector<const char*> Vulkan::getNames(const std::vector<vk::LayerProperties>& layers){
	std::vector<const char*> names(layers.size());

	for(size_t i = 0; i < names.size(); i++){
		names[i] = layers[i].layerName;
	}

	return names;
}





std::vector<vk::ExtensionProperties> Vulkan::getAvailableInstanceExtensions(){
	return vk::enumerateInstanceExtensionProperties();
}

std::vector<vk::ExtensionProperties> Vulkan::getUsedExtensions(	const std::vector<vk::ExtensionProperties>& available, 
																std::vector<vk::ExtensionProperties>& required )
{
	std::vector<vk::ExtensionProperties> extensions;

	//Check for availability of all required extensions
	auto ite = required.begin();
	while(ite != required.end()){
		bool found = false;

		for(const auto& availableExt : available){
			if(std::strncmp(ite->extensionName, availableExt.extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0){
				if(ite->specVersion <= availableExt.specVersion){
					extensions.push_back(availableExt);
					found = true;
					break;
				}
			}
		}

		//Advance to the next one
		if(found){
			ite = required.erase(ite);
		}else {
			++ite;
		}
	}

	return extensions;
}

std::vector<const char*> Vulkan::getNames(const std::vector<vk::ExtensionProperties>& ext){
	std::vector<const char*> names(ext.size());

	for(size_t i = 0; i < names.size(); i++){
		names[i] = ext[i].extensionName;
	}

	return names;
}




bool Vulkan::getPhysicalDeviceSupport(const vk::Instance& instance, const vk::PhysicalDevice& device){
	//Window support is required
	if(Window::getPresentationQueueFamilies(instance, device).size() == 0){
		return false;
	}

	//Check device validation layer support
	auto requiredLayers = getRequiredLayers();
	const auto availableLayers = device.enumerateDeviceLayerProperties();
	getUsedLayers(availableLayers, requiredLayers);
	if(requiredLayers.size()){
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
	auto requiredQueueFamilies = getRequiredQueueFamilies(instance, device);
	const auto availableQueueFamilies = device.getQueueFamilyProperties();
	getUsedQueueFamilies(availableQueueFamilies, requiredQueueFamilies);
	if(requiredQueueFamilies.size()){
		return false;
	}

	return true;
}




bool Vulkan::getQueueFamilyCompatibility(vk::QueueFlags required, vk::QueueFlags available){
	return (required & available) == required;
}

std::vector<vk::QueueFamilyProperties> Vulkan::getUsedQueueFamilies(const std::vector<vk::QueueFamilyProperties>& available, 
																	std::vector<vk::QueueFamilyProperties>& required )
{
	std::vector<vk::QueueFamilyProperties> queueFamilies;

	auto ite = required.begin();
	while(ite != required.end()){
		for(const auto& availableQf : available){
			if(getQueueFamilyCompatibility(ite->queueFlags, availableQf.queueFlags)){
				vk::QueueFamilyProperties queue = availableQf;

				if(ite->queueCount > availableQf.queueCount){
					//Not enough of this queue family. Add the exisiting ones
					queueFamilies.push_back(queue);
					ite->queueCount -= queue.queueCount;
				} else {
					//Add only the required amount
					queue.queueCount = ite->queueCount;
					queueFamilies.push_back(queue);
					ite->queueCount = 0;
					break;
				}
			}
		}

		//Next requirement
		if(ite->queueCount == 0){
			ite = required.erase(ite);
		}else {
			++ite;
		}
	}
	
	return queueFamilies;
}

std::map<uint32_t, uint32_t> Vulkan::getQueueFamilyIndices(	const std::vector<vk::QueueFamilyProperties>& available, 
															const std::vector<vk::QueueFamilyProperties>& requested )
{
	std::map<uint32_t, uint32_t> result;

	for(const auto& req : requested){
		const size_t idx = getQueueFamilyIndex(available, req.queueFlags);
		result[idx] = std::max(result[idx], req.queueCount);
	}

	return result;
}

size_t Vulkan::getQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& qf, vk::QueueFlags flags){
	size_t i;

	for(i = 0; i < qf.size(); i++){
		if(getQueueFamilyCompatibility(flags, qf[i].queueFlags) && (qf[i].queueCount > 0)) {
			return i;
		}
	}

	throw Exception("Requested queue family not found!");
}

std::vector<std::vector<vk::Queue>>	Vulkan::getQueues(	const vk::PhysicalDevice& physicalDevice,
														const vk::Device& dev,
														const std::vector<vk::QueueFamilyProperties>& queueFamilies )
{
	std::vector<std::vector<vk::Queue>> result;
	result.reserve(queueFamilies.size());

	const auto availableQueueFamilies = physicalDevice.getQueueFamilyProperties();

	//Query the queues
	for(const auto& queueFamily : queueFamilies){
		result.emplace_back(std::vector<vk::Queue>(queueFamily.queueCount));

		//Get the indices corresponding to this family
		//It might be distributed among more than one available family
		const auto indices = getQueueFamilyIndices(
			availableQueueFamilies, 
			std::vector<vk::QueueFamilyProperties>(1, queueFamily)
		);

		//Iterate though the returned families and assign them to the queue vector
		size_t j = 0; //Queue # counter
		for(const auto& index : indices){
			for(size_t i = 0; i < index.second; i++){
				result.back()[j] = dev.getQueue(index.first, i);
				j++;
			}
		}
				
		//Array should be full
		assert(j == result.back().size());
	}

	return result;
}

}