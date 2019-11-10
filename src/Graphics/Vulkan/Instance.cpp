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
	const auto extensions = getExtensions(ext);
	const auto extensionNames = getNames(extensions);

	//Get the validation layers
	const auto validationLayers = getValidationLayers(vl);
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

	//Get all the avalible devices
	uint32_t count;
	vkEnumeratePhysicalDevices(m_instance, &count, nullptr);

	std::vector<VkPhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(m_instance, &count, devices.data());

	m_physicalDevices.reserve(count);

	for(auto dev : devices){
		m_physicalDevices.emplace_back(PhysicalDevice(*this, dev));
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

const std::vector<PhysicalDevice>& Instance::getPhysicalDevices() const{
	return m_physicalDevices;
}



std::vector<Extension> Instance::getAvailableExtensions(){
	//Get all the avalible extensions
	uint32_t count;
	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

	std::vector<Extension> extensions(count);
	vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());

	return extensions;
}

std::vector<Extension> Instance::getExtensions(std::vector<Extension> required){
	const auto available = getAvailableExtensions();
	std::vector<Extension> extensions;

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

		if(found){
			ite = required.erase(ite);
		}else {
			++ite;
		}
	}

	if(required.size()){
		//There are missing extensions

		std::string missingNames;
		for(const auto& m : required){
			missingNames += m.extensionName;
			missingNames += "\n";
		}

		throw Exception("Missing Vulkan extensions:\n" + missingNames);
	}

	return extensions;
}

std::vector<const char*> Instance::getNames(const std::vector<Extension>& ext){
	std::vector<const char*> names;
	const size_t nExtensions = ext.size();

	names.reserve(nExtensions);
	for(size_t i = 0; i < nExtensions; i++){
		names.push_back(ext[i].extensionName);
	}

	return names;
}


std::vector<ValidationLayer> Instance::getAvailableValidationLayers(){
	//Get all the available validation layers
	uint32_t count;
	vkEnumerateInstanceLayerProperties(&count, nullptr);

	std::vector<ValidationLayer> validationLayers(count);
	vkEnumerateInstanceLayerProperties(&count, validationLayers.data());

	return validationLayers;
}

std::vector<ValidationLayer> Instance::getValidationLayers(std::vector<ValidationLayer> required){
	const auto available = getAvailableValidationLayers();
	std::vector<ValidationLayer> layers;

	//Check for availability of all required validation layers
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

		if(found){
			ite = required.erase(ite);
		}else {
			++ite;
		}
	}

	if(required.size()){
		//There are missing validation layers

		std::string missingNames;
		for(const auto& m : required){
			missingNames += m.layerName;
			missingNames += "\n";
		}

		throw Exception("Missing Vulkan validation layers:\n" + missingNames);
	}

	return layers;
}

std::vector<const char*> Instance::getNames(const std::vector<ValidationLayer>& ext){
	std::vector<const char*> names;
	const size_t nExtensions = ext.size();

	names.reserve(nExtensions);
	for(size_t i = 0; i < nExtensions; i++){
		names.push_back(ext[i].layerName);
	}

	return names;
}


}