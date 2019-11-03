#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include <vulkan/vulkan.h>

namespace Zuazo::Graphics {

class Vulkan {
public:
	using Extension = VkExtensionProperties;
	using ValidationLayer = VkLayerProperties;

	Vulkan();
	Vulkan(const Vulkan& other) = delete;
	~Vulkan();


private:
	VkInstance							m_instance;
	VkDebugUtilsMessengerEXT			m_messenger;


	void								setupValidationLayers();
	void								cleanValidationLayers();

	template<typename F>
	F									requestFunction(const std::string& name);

	static std::vector<Extension> 		getRequiredExtensions();
	static std::vector<ValidationLayer> getValidationLayers();
};

}