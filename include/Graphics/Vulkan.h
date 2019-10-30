#pragma once

#include "../Zuazo.h"

#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include <vulkan/vulkan.h>

namespace Zuazo::Graphics {

class Vulkan {
	friend void Zuazo::init(ApplicationInfo&& appinfo);
	friend void Zuazo::end();
public:
	using Extension = VkExtensionProperties;
	using ValidationLayer = VkLayerProperties;

	struct Message {
		std::string what;
	};

	Vulkan(const Vulkan& other) = delete;
	~Vulkan();

	std::queue<Message>					getMessages();

	static Vulkan& 						getVulkan();
private:
	Vulkan();

	void								setupValidationLayers();
	void								cleanValidationLayers();

	VkInstance							m_instance;

	VkDebugUtilsMessengerEXT			m_messenger;

	std::mutex 							m_messsageMutex;
	std::queue<Message>					m_messsages;
	template<typename F>
	F									requestFunction(const std::string& name);

	static std::vector<Extension> 		getRequiredExtensions();
	static std::vector<ValidationLayer> getValidationLayers();

	static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerCallback(
												VkDebugUtilsMessageSeverityFlagBitsEXT severity,
												VkDebugUtilsMessageTypeFlagsEXT type,
												const VkDebugUtilsMessengerCallbackDataEXT* data,
												void* userData );

	static std::unique_ptr<Vulkan> 		s_vulkan;
	static void 						init();
	static void 						end();
};

}