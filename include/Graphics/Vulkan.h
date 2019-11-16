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
	std::vector<std::vector<vk::Queue>>				m_queues;
	vk::UniqueDevice								m_device;

	static vk::UniqueInstance						createInstance();
	static UniqueDebugUtilsMessengerEXT				createMessenger(const vk::Instance& instance, const vk::DispatchLoaderDynamic& loader, Vulkan* vk);
	static vk::PhysicalDevice						getBestPhysicalDevice(const vk::Instance& instance);
	static vk::UniqueDevice							createDevice(const vk::Instance& instance, const vk::PhysicalDevice& physicalDevice, std::vector<std::vector<vk::Queue>>& queues);

	static std::vector<vk::LayerProperties> 		getRequiredLayers();
	static std::vector<vk::ExtensionProperties>		getRequiredInstanceExtensions();
	static std::vector<vk::QueueFamilyProperties>	getRequiredQueueFamilies(const vk::Instance& inst, const vk::PhysicalDevice& dev);
	static std::vector<vk::ExtensionProperties>		getRequiredDeviceExtensions();

	static bool										getPhysicalDeviceSupport(const vk::Instance& instance, const vk::PhysicalDevice& device);
};

}