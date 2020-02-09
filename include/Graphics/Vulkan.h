#pragma once

#include "../Version.h"
#include "../Verbosity.h"
#include "../Utils/BufferView.h"

#include <vector>
#include <array>
#include <map>
#include <optional>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_DEFAULT_DISPATCHER void()
#include <vulkan/vulkan.hpp>

#include <cstdint>

namespace Zuazo::Graphics {

class Vulkan {
public:
	using DeviceScoreFunc = std::function<uint32_t(const vk::DispatchLoaderDynamic&, vk::PhysicalDevice)>;

	Vulkan(	const char* appName, 
			uint32_t version,
			const DeviceScoreFunc& scoreFunc );
	Vulkan(const Vulkan& other) = delete;
	~Vulkan() = default;

	const vk::DynamicLoader&						getLoader() const;
	const vk::DispatchLoaderDynamic&				getDispatcher() const;
	vk::Instance									getInstance() const;
	vk::PhysicalDevice								getPhysicalDevice() const;
	vk::Device										getDevice() const;
	uint32_t										getGraphicsQueueIndex() const;
	vk::Queue										getGraphicsQueue() const;
	vk::CommandPool									getGraphicsCommandPool() const;
	uint32_t										getComputeQueueIndex() const;
	vk::Queue										getComputeQueue() const;
	vk::CommandPool									getComputeCommandPool() const;
	uint32_t										getTransferQueueIndex() const;
	vk::Queue										getTransferQueue() const;
	vk::CommandPool									getTransferCommandPool() const;
	uint32_t										getPresentationQueueIndex() const;
	vk::Queue										getPresentationQueue() const;
	vk::CommandPool									getPresentationCommandPool() const;

	vk::FormatProperties							getFormatFeatures(vk::Format format) const;

	vk::UniqueShaderModule							getShader(const Utils::BufferView<uint32_t>& code);
private:
	enum QueueIndices {
		GRAPHICS_QUEUE,
		COMPUTE_QUEUE,
		TRANSFER_QUEUE,
		PRESENTATION_QUEUE,
		QUEUE_NUM
	};

	vk::DynamicLoader								m_loader;
	vk::DispatchLoaderDynamic						m_dispatcher;
	vk::UniqueInstance								m_instance;
	vk::UniqueDebugUtilsMessengerEXT				m_messenger;
	vk::PhysicalDevice								m_physicalDevice;
	std::array<uint32_t, QUEUE_NUM>					m_queueIndices;
	vk::UniqueDevice								m_device;
	std::array<vk::Queue, QUEUE_NUM>				m_queues;
	std::array<vk::UniqueCommandPool, QUEUE_NUM>	m_commandPools;

	static vk::DispatchLoaderDynamic				createDispatcher(const vk::DynamicLoader& loader);
	static vk::UniqueInstance						createInstance(	vk::DispatchLoaderDynamic& disp, 
																	const char* appName, 
																	uint32_t appVersion );
	static vk::UniqueDebugUtilsMessengerEXT			createMessenger(const vk::DispatchLoaderDynamic& disp, 
																	const vk::Instance& instance );
	static vk::PhysicalDevice						getBestPhysicalDevice(	const vk::DispatchLoaderDynamic& disp, 
																			const vk::Instance& instance, 
																			const DeviceScoreFunc& scoreFunc );
	static std::array<uint32_t, QUEUE_NUM>			getQueueIndices(const vk::DispatchLoaderDynamic& disp, 
																	const vk::Instance& inst, 
																	const vk::PhysicalDevice& dev );
	static vk::UniqueDevice							createDevice(	vk::DispatchLoaderDynamic& disp, 
																	const vk::PhysicalDevice& physicalDevice, 
																	const std::array<uint32_t, QUEUE_NUM>& queueIndices );
	static std::array<vk::Queue, QUEUE_NUM>			getQueues(	const vk::DispatchLoaderDynamic& disp, 
																const vk::Device& device, 
																const std::array<uint32_t, QUEUE_NUM>& queueIndices);
	static std::array<vk::UniqueCommandPool, QUEUE_NUM> createCommandPools( const vk::DispatchLoaderDynamic& disp, 
																			const vk::Device& device, 
																			const std::array<uint32_t, QUEUE_NUM>& queueIndices );
	static std::vector<vk::LayerProperties> 		getRequiredLayers();
	static std::vector<vk::ExtensionProperties>		getRequiredInstanceExtensions();
	static std::vector<vk::ExtensionProperties>		getRequiredDeviceExtensions();
	static std::vector<vk::QueueFamilyProperties> 	getRequiredQueueFamilies();
};

}