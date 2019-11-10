#include <Graphics/Vulkan.h>

#include <Graphics/Window.h>
#include <Macros.h>
#include <Zuazo.h>
#include <Exception.h>

#include <algorithm>
#include <vector>
#include <cstring>
#include <iostream>
#include <set>

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


namespace Zuazo::Graphics {


/*
 * Vulkan::Instance
 */

Vulkan::Instance::Instance(){
	const Zuazo::ApplicationInfo& zzAppInfo = getApplicationInfo();

	//Fill the application information
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = zzAppInfo.name.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(zzAppInfo.version[0], zzAppInfo.version[1], zzAppInfo.version[2]);
	appInfo.pEngineName = "Zuazo";
	appInfo.engineVersion = 0; VK_MAKE_VERSION(Zuazo::version[0], Zuazo::version[1], Zuazo::version[2]);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//Get the required extensions
	const auto extensions = getExtensions();
	const auto extensionNames = getNames(extensions);

	//Get the validation layers
	const auto validationLayers = getValidationLayers();
	const auto validationLayerNames = getNames(validationLayers);

	//Fill the instace information
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = extensionNames.size();
	createInfo.ppEnabledExtensionNames = extensionNames.data();
	createInfo.enabledLayerCount = validationLayerNames.size();
	createInfo.ppEnabledLayerNames = validationLayerNames.data();

	//Finally create the Vulkan instance
	VkResult err = vkCreateInstance(&createInfo, nullptr, &m_instance);

	switch(err){
		case VK_SUCCESS:
			break; //Everything OK.

		default:
			throw Exception("Error creating a Vulkan instance");
	}
}

Vulkan::Instance::~Instance(){
	if(m_instance){
		vkDestroyInstance(m_instance, nullptr);
	}
}




VkInstance Vulkan::Instance::get(){
	return m_instance;
}



std::vector<Vulkan::PhysicalDevice> Vulkan::Instance::getPhysicalDevices() const{
	//Get all the avalible devices
	uint32_t count;
	vkEnumeratePhysicalDevices(m_instance, &count, nullptr);

	std::vector<PhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(m_instance, &count, devices.data());

	return devices;
}



std::vector<Vulkan::Extension> Vulkan::Instance::getAvailableExtensions(){
	//Get all the avalible extensions
	uint32_t count;
	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

	std::vector<Extension> extensions(count);
	vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());

	return extensions;
}

std::vector<Vulkan::Extension> Vulkan::Instance::getExtensions(){
	const auto required = getRequiredExtensions();
	const auto available = getAvailableExtensions();
	std::vector<Extension> extensions;

	//Check for availability of all required extensions
	for(const auto& requiredExt : required){
		for(const auto& availableExt : available){
			if(std::strncmp(requiredExt.extensionName, availableExt.extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0){
				if(requiredExt.specVersion <= availableExt.specVersion){
					extensions.push_back(availableExt);
					break;
				}
			}
		}
	}

	if(extensions.size() != required.size()){
		//There are missing extensions
		std::vector<Extension> missing;

		//Get all the missing extensions
		for(const auto& requiredExt : required){
			bool found = false;

			for(const auto& ext : extensions){
				if(std::strncmp(requiredExt.extensionName, ext.extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0){
					found = true;
					break;
				}
			}

			if(!found){
				missing.push_back(requiredExt);
			}
		}

		std::string missingNames;

		for(const auto& m : missing){
			missingNames += m.extensionName;
			missingNames += "\n";
		}

		throw Exception("Missing Vulkan extensions:\n" + missingNames);
	}

	return extensions;
}

std::vector<const char*> Vulkan::Instance::getNames(const std::vector<Extension>& ext){
	std::vector<const char*> names;
	const size_t nExtensions = ext.size();

	names.reserve(nExtensions);
	for(size_t i = 0; i < nExtensions; i++){
		names.push_back(ext[i].extensionName);
	}

	return names;
}





std::vector<Vulkan::ValidationLayer> Vulkan::Instance::getAvailableValidationLayers(){
	//Get all the available validation layers
	uint32_t count;
	vkEnumerateInstanceLayerProperties(&count, nullptr);

	std::vector<ValidationLayer> validationLayers(count);
	vkEnumerateInstanceLayerProperties(&count, validationLayers.data());

	return validationLayers;
}

std::vector<Vulkan::ValidationLayer> Vulkan::Instance::getValidationLayers(){
	const auto required = getRequieredValidationLayers();
	const auto available = getAvailableValidationLayers();
	std::vector<ValidationLayer> layers;

	//Check for availability of all required validation layers
	for(const auto& requiredVl : required){
		for(const auto& availableVl : available){
			if(std::strncmp(requiredVl.layerName, availableVl.layerName, VK_MAX_EXTENSION_NAME_SIZE) == 0){
				if(requiredVl.specVersion <= availableVl.specVersion){
					layers.push_back(availableVl);
					break;
				}
			}
		}
	}

	if(layers.size() != required.size()){
		//There are missing validation layers
		std::vector<ValidationLayer> missing;

		//Get all the missing extensions
		for(const auto& requiredVl : required){
			bool found = false;

			for(const auto& vl : layers){
				if(std::strncmp(requiredVl.layerName, vl.layerName, VK_MAX_EXTENSION_NAME_SIZE) == 0){
					found = true;
					break;
				}
			}

			if(!found){
				missing.push_back(requiredVl);
			}
		}

		std::string missingNames;
		for(const auto& m : missing){
			missingNames += m.layerName;
			missingNames += "\n";
		}

		throw Exception("Missing Vulkan validation layers:\n" + missingNames);
	}

	return layers;
}

std::vector<const char*> Vulkan::Instance::getNames(const std::vector<ValidationLayer>& vl){
	std::vector<const char*> names;
	const size_t nLayers = vl.size();

	names.reserve(nLayers);
	for(size_t i = 0; i < nLayers; i++){
		names.push_back(vl[i].layerName);
	}

	return names;
}



/*
 * Vulkan::Messenger
 */


Vulkan::Messenger::Messenger(Instance& instance) :
	m_instance(instance)
{
	if(getApplicationInfo().isDebug) {
		//Request the functions to Vulkan
		auto vkCreateDebugUtilsMessengerEXT = 
			m_instance.getFunction<PFN_vkCreateDebugUtilsMessengerEXT>("vkCreateDebugUtilsMessengerEXT");

		//Setup validation layer callback
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = 
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | 
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		createInfo.messageType = 
					VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = validationLayerCallback;
		createInfo.pUserData = this;

		//Create the messenger
		VkResult err = vkCreateDebugUtilsMessengerEXT(m_instance.get(), &createInfo, nullptr, &m_messenger);

		switch(err){
			case VK_SUCCESS:
				break; //Everything OK.

			default:
				throw Exception("Error creating Vulkan validation layer messenger");
		}
	}
}

Vulkan::Messenger::~Messenger() {
	if(m_messenger){
		//Request the functions to Vulkan
		auto vkDestroyDebugUtilsMessengerEXT = 
			m_instance.getFunction<PFN_vkDestroyDebugUtilsMessengerEXT>("vkDestroyDebugUtilsMessengerEXT");

		vkDestroyDebugUtilsMessengerEXT(m_instance.get(), m_messenger, nullptr);
	}
}




VkDebugUtilsMessengerEXT Vulkan::Messenger::get() {
	return m_messenger;
}


/*
 * Vulkan::Device
 */

Vulkan::Device::Device(PhysicalDevice dev){
	const auto availableQueueFamilies = getAvailableQueueFamilies(dev);
	const auto graphicsQueueIdx = getQueueFamilyIndex(availableQueueFamilies, VK_QUEUE_GRAPHICS_BIT);
	const auto computeQueueIdx = getQueueFamilyIndex(availableQueueFamilies, VK_QUEUE_COMPUTE_BIT);
	const auto transferQueueIdx = getQueueFamilyIndex(availableQueueFamilies, VK_QUEUE_TRANSFER_BIT);

	const std::set<size_t> indices {
		graphicsQueueIdx,
		computeQueueIdx,
		transferQueueIdx
	};

	const float queuePriorities = 1.0f;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for(auto ite = indices.cbegin(); ite != indices.cend(); ++ite){
		VkDeviceQueueCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = *ite;
		createInfo.queueCount = 1;
		createInfo.pQueuePriorities = &queuePriorities;

		queueCreateInfos.push_back(createInfo);
	}

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = queueCreateInfos.size();


	//Create the logical device
	VkResult err = vkCreateDevice(dev, &createInfo, nullptr, &m_device);

	switch(err){
		case VK_SUCCESS:
			break; //Everything OK.

		default:
			throw Exception("Error creating Vulkan device");
	}

	//Fill the queues
	vkGetDeviceQueue(m_device, graphicsQueueIdx, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, computeQueueIdx, 0, &m_computeQueue);
	vkGetDeviceQueue(m_device, transferQueueIdx, 0, &m_transferQueue);
}

Vulkan::Device::~Device(){
	if(m_device){
		vkDestroyDevice(m_device, nullptr);
	}
}



VkDevice Vulkan::Device::get(){
	return m_device;
}

VkQueue Vulkan::Device::getGraphicsQueue(){
	return m_graphicsQueue;
}

VkQueue Vulkan::Device::getComputeQueue(){
	return m_computeQueue;
}

VkQueue Vulkan::Device::getTranseferQueue(){
	return m_transferQueue;
}




std::vector<Vulkan::QueueFamily> Vulkan::Device::getAvailableQueueFamilies(PhysicalDevice dev){
	//Get all the available validation layers
	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);

	std::vector<QueueFamily> queueFamilies(count);
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, queueFamilies.data());

	return queueFamilies;
}

size_t Vulkan::Device::getQueueFamilyIndex(const std::vector<QueueFamily>& qf, uint32_t flags){
	for(size_t i = 0; i < qf.size(); i++){
		if((qf[i].queueFlags & flags) && qf[i].queueCount > 0) {
			return i;
		}
	}

	throw Exception("Required Vulkan queue family not found");
}


/*
 * Vulkan::Vulkan
 */

std::function<int32_t(const Vulkan::PhysicalDeviceInfo&)> Vulkan::deviceScoreFunc;

Vulkan::Vulkan() :
	m_instance(),
	m_messenger(m_instance),
	m_device(selectDevice(m_instance.getPhysicalDevices()))
{
}

Vulkan::Instance& Vulkan::getInstance(){
	return m_instance;
}

Vulkan::Messenger& Vulkan::getMessenger(){
	return m_messenger;
}

Vulkan::Device& Vulkan::getDevice(){
	return m_device;
}




std::vector<Vulkan::Extension> Vulkan::getRequiredExtensions(){
	std::vector<Extension> extensions;

	//Add window swap-chain extensions
	const auto windowExtensions = Window::getRequiredVulkanExtensions();
	std::copy(
		windowExtensions.cbegin(),
		windowExtensions.cend(),
		std::back_insert_iterator(extensions)
	);

	if(Zuazo::getApplicationInfo().isDebug){
		//Add debug utils extension requirement
		Extension ext = {};
		std::strncpy(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
		extensions.push_back(ext);
	}

	return extensions;
}

std::vector<Vulkan::ValidationLayer> Vulkan::getRequieredValidationLayers(){
	std::vector<ValidationLayer> validationLayers;

	if(Zuazo::getApplicationInfo().isDebug){
		//Add debug utils extension requirement
		//Khronos validation layer
		ValidationLayer khronosValidation = {};
		std::strncpy(khronosValidation.layerName, "VK_LAYER_KHRONOS_validation", VK_MAX_EXTENSION_NAME_SIZE);
		validationLayers.push_back(khronosValidation);
	}

	return validationLayers;
}

Vulkan::PhysicalDevice Vulkan::selectDevice(const std::vector<PhysicalDevice>& devices){
	std::pair<PhysicalDevice, int> best = {{}, -1};

	for(const auto& device : devices){
		const int deviceScore = getScore(device);
		if(deviceScore > best.second){
			best = {device, deviceScore};
		}
	}

	if(best.second < 0){
		throw Exception("No compatible GPU cards were found");
	}

	return best.first;
}

int32_t Vulkan::getScore(const PhysicalDevice& dev){
	PhysicalDeviceInfo deviceInfo;

	//Get properties and features
	vkGetPhysicalDeviceProperties(dev, &deviceInfo.properties);
	vkGetPhysicalDeviceFeatures(dev, &deviceInfo.features);

	if(!isDeviceSupported(deviceInfo)){
		return -1; //Not suppoted
	}

	if(deviceScoreFunc){
		return deviceScoreFunc(deviceInfo);
	} else {
		return defaultDeviceScoreFunc(deviceInfo);
	}
}

bool Vulkan::isDeviceSupported(const PhysicalDeviceInfo& devInfo){
	ZUAZO_IGNORE_PARAM(devInfo);
	return true;
}

int32_t Vulkan::defaultDeviceScoreFunc(const PhysicalDeviceInfo& devInfo){
	int32_t score = 0;

	if(devInfo.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
		score += 1024;
	}

	score += devInfo.properties.limits.maxImageDimension2D;

	return score;
}

}