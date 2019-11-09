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

	class Instance {
	public:
		Instance();
		Instance(const Instance& other) = delete;
		~Instance();

		template<typename F>
		F									requestFunction(const std::string& name) const;

		VkInstance							get();
	private:
		VkInstance							m_instance = nullptr;

		static std::vector<Extension>		getSupportedExtensions();
		static std::vector<Extension> 		getExtensions();
		static std::vector<const char*>		getNames(const std::vector<Extension>& ext);

		static std::vector<ValidationLayer> getSupportedValidationLayers();
		static std::vector<ValidationLayer> getValidationLayers();
		static std::vector<const char*>		getNames(const std::vector<ValidationLayer>& vl);
	};

	class Messenger{
	public:
		Messenger(Instance& instance);
		Messenger(const Messenger& other) = delete;
		~Messenger();

		VkDebugUtilsMessengerEXT			get();
	private:
		Instance&							m_instance;

		VkDebugUtilsMessengerEXT 			m_messenger = nullptr;
	};

	

	Vulkan();
	Vulkan(const Vulkan& other) = delete;
	~Vulkan() = default;

	Instance&							getInstance();
private:
	Instance							m_instance;
	Messenger							m_messenger;

	static std::vector<Extension>		getRequiredExtensions();

	static std::vector<ValidationLayer> getRequieredValidationLayers();
};

}

#include "Vulkan.inl"
