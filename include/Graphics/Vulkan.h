#pragma once

#include <vector>
#include <array>
#include <map>
#include <optional>

#include <vulkan/vulkan.hpp>

namespace Zuazo::Graphics {

class Vulkan {
public:
	using DeviceScoreFunc = std::function<uint32_t(const vk::PhysicalDevice&)>;

	Vulkan();
	Vulkan(const Vulkan& other) = delete;
	~Vulkan() = default;

	const vk::Instance&								getInstance() const;
	const vk::PhysicalDevice&						getPhysicalDevice() const;
	const vk::Device&								getDevice() const;
	const vk::Queue&								getGraphicsQueue() const;
	const vk::Queue&								getComputeQueue() const;
	const vk::Queue&								getTransferQueue() const;
	const vk::Queue&								getPresentationQueue() const;

	static DeviceScoreFunc 							deviceScoreFunc;

private:
	enum QueueIndices {
		GRAPHICS_QUEUE,
		COMPUTE_QUEUE,
		TRANSFER_QUEUE,
		PRESENTATION_QUEUE,
		QUEUE_NUM
	};

	using UniqueDebugUtilsMessengerEXT = vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>;

	vk::UniqueInstance								m_instance;
	vk::DispatchLoaderDynamic						m_loader;
	UniqueDebugUtilsMessengerEXT					m_messenger;
	vk::PhysicalDevice								m_physicalDevice;
	vk::UniqueDevice								m_device;
	std::array<vk::Queue, QUEUE_NUM>				m_queues;

	static vk::Instance								createInstance();
	static UniqueDebugUtilsMessengerEXT				createMessenger(const vk::Instance& instance, const vk::DispatchLoaderDynamic& loader);
	static vk::PhysicalDevice						getBestPhysicalDevice(const vk::Instance& instance);
	static vk::Device								createDevice(const vk::Instance& instance, const vk::PhysicalDevice& physicalDevice);
	static std::array<vk::Queue, QUEUE_NUM>			getQueues(const vk::Device& dev);

	static std::vector<vk::LayerProperties> 		getRequiredLayers();
	static std::vector<vk::ExtensionProperties>		getRequiredInstanceExtensions();
	static std::vector<vk::QueueFamilyProperties>	getRequiredQueueFamilies(const vk::Instance& inst, const vk::PhysicalDevice& dev);
	static std::vector<vk::ExtensionProperties>		getRequiredDeviceExtensions();

	static std::vector<vk::LayerProperties>			getAvailableInstanceLayers();
	static std::vector<vk::LayerProperties> 		getUsedLayers(const std::vector<vk::LayerProperties>& available, std::vector<vk::LayerProperties>& required);
	static std::vector<const char*> 				getNames(const std::vector<vk::LayerProperties>& layer);

	static std::vector<vk::ExtensionProperties>		getAvailableInstanceExtensions();
	static std::vector<vk::ExtensionProperties> 	getUsedExtensions(const std::vector<vk::ExtensionProperties>& available, std::vector<vk::ExtensionProperties>& required);
	static std::vector<const char*> 				getNames(const std::vector<vk::ExtensionProperties>& ext);

	static bool										getPhysicalDeviceSupport(const vk::Instance& instance, const vk::PhysicalDevice& device);

	static std::vector<vk::QueueFamilyProperties>	getUsedQueueFamilies(const std::vector<vk::QueueFamilyProperties>& available, std::vector<vk::QueueFamilyProperties>& required);
	static bool										getQueueFamilyCompatibility(vk::QueueFlags required, vk::QueueFlags available);
	static std::map<uint32_t, uint32_t>				getQueueFamilyIndices(const std::vector<vk::QueueFamilyProperties>& available, const std::vector<vk::QueueFamilyProperties>& requested);
	static size_t									getQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& qf, vk::QueueFlags flags);
};

}