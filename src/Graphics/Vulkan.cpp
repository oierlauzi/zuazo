#include <Graphics/Vulkan.h>

#include <Graphics/Window.h>

#include <algorithm>
#include <vector>
#include <cstring>
#include <iostream>

namespace Zuazo::Graphics {

std::unique_ptr<Vulkan> Vulkan::s_vulkan;

Vulkan& Vulkan::getVulkan(){
	return *s_vulkan;
}

Vulkan::Vulkan() :
	m_instance(nullptr),
	m_messenger(nullptr)
{
	const Zuazo::ApplicationInfo& zzAppInfo = Zuazo::getApplicationInfo();

	//Fill the application information
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = zzAppInfo.name.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(zzAppInfo.version[0], zzAppInfo.version[1], zzAppInfo.version[2]);
	appInfo.pEngineName = "Zuazo";
	appInfo.engineVersion = 0; VK_MAKE_VERSION(Zuazo::version[0], Zuazo::version[1], Zuazo::version[2]);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//Copy the required extensions
	auto requiredExtensions = getRequiredExtensions();
	std::vector<const char*> requiredExtensionNames(requiredExtensions.size());
	for(size_t i = 0; i < requiredExtensions.size(); i++){
		requiredExtensionNames[i] = requiredExtensions[i].extensionName;
	}

	//Copy the validation layers
	auto validationLayers = getValidationLayers();
	std::vector<const char*> validationLayerNames(validationLayers.size());
	for(size_t i = 0; i < validationLayers.size(); i++){
		validationLayerNames[i] = validationLayers[i].layerName;
	}

	//Fill the instace information
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = requiredExtensionNames.size();
	createInfo.ppEnabledExtensionNames = requiredExtensionNames.data();
	createInfo.enabledLayerCount = validationLayerNames.size();
	createInfo.ppEnabledLayerNames = validationLayerNames.data();

	//Finally create the Vulkan instance
	VkResult err = vkCreateInstance(&createInfo, nullptr, &m_instance);

	switch(err){
		case VK_SUCCESS:
			break; //Everything OK.

		default:
			//Unexpected error
			break;
	}

	if(validationLayers.size()){
		setupValidationLayers();
	}
}

Vulkan::~Vulkan(){
	if(m_instance){
		if(m_messenger) {
			cleanValidationLayers();
		}

		vkDestroyInstance(m_instance, nullptr);
	}
}

std::queue<Vulkan::Message> Vulkan::getMessages(){
	std::lock_guard<std::mutex> lock(m_messsageMutex);
	return std::move(m_messsages);
}

void Vulkan::setupValidationLayers(){
	//Request the functions to Vulkan
	auto vkCreateDebugUtilsMessengerEXT = 
		requestFunction<PFN_vkCreateDebugUtilsMessengerEXT>("vkCreateDebugUtilsMessengerEXT");

	if(vkCreateDebugUtilsMessengerEXT) {
		//Setup validation layer callback
		VkDebugUtilsMessengerCreateInfoEXT layerInfo = {};
		layerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		layerInfo.messageSeverity = 
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | 
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		layerInfo.messageType = 
					VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		layerInfo.pfnUserCallback = validationLayerCallback;
		layerInfo.pUserData = this;

		//Create the messenger
		VkResult err = vkCreateDebugUtilsMessengerEXT(m_instance, &layerInfo, nullptr, &m_messenger);

		switch(err){
		case VK_SUCCESS:
			break; //Everything OK.

		default:
			//Unexpected error
			break;
		}
	} else {
		//Extension !found
	}
}

void Vulkan::cleanValidationLayers(){
	//Request the functions to Vulkan
	auto vkDestroyDebugUtilsMessengerEXT = 
		requestFunction<PFN_vkDestroyDebugUtilsMessengerEXT>("vkDestroyDebugUtilsMessengerEXT");

	if(vkDestroyDebugUtilsMessengerEXT) {
		vkDestroyDebugUtilsMessengerEXT(m_instance, m_messenger, nullptr);
	} else {
		//Extension !found
	}
}

template<typename F>
F Vulkan::requestFunction(const std::string& name){
	return reinterpret_cast<F>(vkGetInstanceProcAddr(m_instance, name.c_str()));
}

std::vector<Vulkan::Extension> Vulkan::getRequiredExtensions(){
	std::vector<Vulkan::Extension> extensions;

	//Add the window extensions
	auto windowExtensions = Window::getRequiredVulkanExtensions();
	std::copy(
		windowExtensions.cbegin(),
		windowExtensions.cend(),
		std::back_insert_iterator(extensions)
	);

	//Add debug extension
	if(getApplicationInfo().isDebug){
		Extension ext;
		strncpy(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
		ext.specVersion = 0;
		extensions.push_back(ext);
	}

	return extensions;
}

std::vector<Vulkan::ValidationLayer> Vulkan::getValidationLayers(){
	std::vector<ValidationLayer> validationLayers;

	if(getApplicationInfo().isDebug){
		const std::vector<const char*> interestedValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};		

		//Get all the avalible validation layers
		uint32_t count;
		vkEnumerateInstanceLayerProperties(&count, nullptr);
		std::vector<ValidationLayer> avalibleValidationLayers(count);
		vkEnumerateInstanceLayerProperties(&count, avalibleValidationLayers.data());

		for(auto interested : interestedValidationLayers){
			for(const auto& avalible : avalibleValidationLayers){
				if(strncmp(interested, avalible.layerName, VK_MAX_EXTENSION_NAME_SIZE) == 0){
					validationLayers.emplace_back(avalible);
				}
			}
		}
	}

	return validationLayers;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::validationLayerCallback(
											VkDebugUtilsMessageSeverityFlagBitsEXT severity,
											VkDebugUtilsMessageTypeFlagsEXT type,
											const VkDebugUtilsMessengerCallbackDataEXT* data,
											void* userData ) 
{
	Vulkan* instance = static_cast<Vulkan*>(userData);
	Message err{
		.what = std::string(data->pMessage)
	};

	#if !defined(NDEBUG)
		std::cerr << "Vulkan validation layer: " << data->pMessage << std::endl;
	#endif


	std::lock_guard<std::mutex> lock(instance->m_messsageMutex);
	instance->m_messsages.emplace(std::move(err));
	return VK_FALSE;
}

void Vulkan::init(){
	s_vulkan = std::unique_ptr<Vulkan>(new Vulkan);
}

void Vulkan::end(){
	s_vulkan.reset();
}

}