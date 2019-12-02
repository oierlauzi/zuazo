#pragma once

#include "../Zuazo.h"

#include <vector>
#include <array>
#include <map>
#include <optional>

#include <vulkan/vulkan.hpp>

namespace Zuazo::Graphics {

class Vulkan {
public:
	enum class Verbosity {
		DISABLE_VALIDATION_LAYERS,
		ENABLE_VALIDATION_LAYERS
	};

	using DeviceScoreFunc = std::function<uint32_t(const vk::PhysicalDevice&)>;

	Vulkan(const std::string& appName, const Version& appVersion, Verbosity verbosity, const DeviceScoreFunc& scoreFunc);
	Vulkan(const Vulkan& other) = delete;
	~Vulkan() = default;

	const vk::Instance&								getInstance() const;
	const vk::PhysicalDevice&						getPhysicalDevice() const;
	const vk::Device&								getDevice() const;
	const vk::Queue&								getGraphicsQueue() const;
	uint32_t										getGraphicsQueueIndex() const;
	const vk::Queue&								getComputeQueue() const;
	uint32_t										getComputeQueueIndex() const;
	const vk::Queue&								getTransferQueue() const;
	uint32_t										getTransferQueueIndex() const;
	const vk::Queue&								getPresentationQueue() const;
	uint32_t										getPresentationQueueIndex() const;

	static uint32_t									defaultDeviceScoreFunc(const vk::PhysicalDevice& physicalDevice);
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
	std::array<uint32_t, QUEUE_NUM>					m_queueIndices;
	vk::UniqueDevice								m_device;
	std::array<vk::Queue, QUEUE_NUM>				m_queues;

	static vk::UniqueInstance						createInstance(const std::string& appName, const Version& appVersion, Verbosity verbosity);
	static UniqueDebugUtilsMessengerEXT				createMessenger(const vk::Instance& instance, const vk::DispatchLoaderDynamic& loader, Verbosity verbosity);
	static vk::PhysicalDevice						getBestPhysicalDevice(const vk::Instance& instance, const DeviceScoreFunc& scoreFunc);
	static std::array<uint32_t, QUEUE_NUM>			getQueueIndices(const vk::Instance& inst, const vk::PhysicalDevice& dev);
	static vk::UniqueDevice							createDevice(const vk::PhysicalDevice& physicalDevice, const std::array<uint32_t, QUEUE_NUM>& queueIndices);
	static std::array<vk::Queue, QUEUE_NUM>			getQueues(const vk::Device& device, const std::array<uint32_t, QUEUE_NUM>& queueIndices);

	static std::vector<vk::LayerProperties> 		getRequiredLayers(Verbosity verbosity);
	static std::vector<vk::ExtensionProperties>		getRequiredInstanceExtensions(Verbosity verbosity);
	static std::vector<vk::ExtensionProperties>		getRequiredDeviceExtensions();
	static std::vector<vk::QueueFamilyProperties> 	getRequiredQueueFamilies();

	static bool										getPhysicalDeviceSupport(const vk::Instance& instance, const vk::PhysicalDevice& device);
};

}