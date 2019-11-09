#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include <utility>
#include <functional>

#include <vulkan/vulkan.h>

namespace Zuazo::Graphics {

class Vulkan {
public:
	using Extension = VkExtensionProperties;
	using ValidationLayer = VkLayerProperties;
	using PhysicalDevice = VkPhysicalDevice;
	using QueueFamily = VkQueueFamilyProperties;

	struct PhysicalDeviceInfo {
		VkPhysicalDeviceProperties 				properties;
		VkPhysicalDeviceFeatures				features;
	};

	class Instance {
	public:
		Instance();
		Instance(const Instance& other) = delete;
		~Instance();

		template<typename F>
		F									getFunction(const std::string& name) const;

		VkInstance							get();

		std::vector<PhysicalDevice>			getPhysicalDevices() const;
	private:
		VkInstance							m_instance = nullptr;

		static std::vector<Extension>		getAvailableExtensions();
		static std::vector<Extension> 		getExtensions();
		static std::vector<const char*>		getNames(const std::vector<Extension>& ext);

		static std::vector<ValidationLayer> getAvailableValidationLayers();
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


	class Device {
	public:
		Device(PhysicalDevice dev);
		Device(const Device& other) = delete;
		~Device();

		VkDevice							get();
		VkQueue								getGraphicsQueue();
		VkQueue								getComputeQueue();
		VkQueue								getTranseferQueue();

	private:
		VkDevice							m_device = nullptr;
		VkQueue								m_graphicsQueue = nullptr;
		VkQueue								m_computeQueue = nullptr;
		VkQueue								m_transferQueue = nullptr;

		static std::vector<QueueFamily>		getAvailableQueueFamilies(PhysicalDevice dev);
		static size_t						getQueueFamilyIndex(const std::vector<QueueFamily>& qf, uint32_t flags);
	};
	

	Vulkan();
	Vulkan(const Vulkan& other) = delete;
	~Vulkan() = default;

	Instance&							getInstance();
	Messenger&							getMessenger();
	Device&								getDevice();

	static std::function<int32_t(const PhysicalDeviceInfo&)> deviceScoreFunc;
private:
	Instance							m_instance;
	Messenger							m_messenger;
	Device								m_device;

	static std::vector<Extension>		getRequiredExtensions();

	static std::vector<ValidationLayer> getRequieredValidationLayers();

	static PhysicalDevice				selectDevice(const std::vector<PhysicalDevice>& devices);
	static int32_t						getScore(const PhysicalDevice& dev);
	static bool							isDeviceSupported(const PhysicalDeviceInfo& dev);

	static int32_t						defaultDeviceScoreFunc(const PhysicalDeviceInfo& dev);
};

}

#include "Vulkan.inl"
