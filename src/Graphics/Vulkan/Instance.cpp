#include <Graphics/Vulkan/Instance.h>

#include <Zuazo.h>
#include <Exception.h>
#include <Graphics/Vulkan/PhysicalDevice.h>

#include <cstring>

namespace Zuazo::Graphics::Vulkan {

Instance::Instance(const std::vector<Extension>& ext, const std::vector<ValidationLayer>& vl){
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
	const auto availableExtensions = getAvailableExtensions();
	const auto extensions = getUsedExtensions(availableExtensions, ext);
	const auto extensionNames = getNames(extensions);

	//Get the validation layers
	const auto availableValidationLayers = getAvailableValidationLayers();
	const auto validationLayers = getUsedValidationLayers(availableValidationLayers, vl);
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

Instance::~Instance(){
	if(m_instance){
		vkDestroyInstance(m_instance, nullptr);
	}
}



VkInstance Instance::get(){
	return m_instance;
}

std::vector<PhysicalDevice> Instance::getPhysicalDevices() const{
	const auto availableDevices = getPhysicalDevices(m_instance);

	//Insert them on the vector
	std::vector<PhysicalDevice> physicalDevices;
	physicalDevices.reserve(availableDevices.size());
	for(auto dev : availableDevices){
		physicalDevices.emplace_back(PhysicalDevice(dev));
	}

	return physicalDevices;
}





std::vector<Extension> Instance::getAvailableExtensions(){
	//Get all the avalible extensions
	uint32_t count;
	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

	std::vector<Extension> extensions(count);
	vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());

	return extensions;
}


std::vector<ValidationLayer> Instance::getAvailableValidationLayers(){
	//Get all the available validation layers
	uint32_t count;
	vkEnumerateInstanceLayerProperties(&count, nullptr);

	std::vector<ValidationLayer> validationLayers(count);
	vkEnumerateInstanceLayerProperties(&count, validationLayers.data());

	return validationLayers;
}

std::vector<VkPhysicalDevice> Instance::getPhysicalDevices(VkInstance inst){
	//Get all the avalible devices
	uint32_t count;
	vkEnumeratePhysicalDevices(inst, &count, nullptr);

	std::vector<VkPhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(inst, &count, devices.data());

	return devices;
}

}