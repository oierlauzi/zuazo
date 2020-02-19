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

	struct FormatSupport {
		std::vector<vk::Format>	sampler;
		std::vector<vk::Format>	framebuffer;
	};

	static constexpr size_t SAMPLER_COUNT = 4;

	Vulkan(	const std::string_view& appName, 
			Version appVersion,
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
	uint32_t										getComputeQueueIndex() const;
	vk::Queue										getComputeQueue() const;
	uint32_t										getTransferQueueIndex() const;
	vk::Queue										getTransferQueue() const;
	uint32_t										getPresentationQueueIndex() const;
	vk::Queue										getPresentationQueue() const;
	const FormatSupport&							getFormatSupport() const;
	vk::Sampler										getSampler(	size_t index, 
																vk::Filter filter) const;
	
	vk::FormatProperties							getFormatFeatures(vk::Format format) const;

	vk::UniqueSwapchainKHR							createSwapchain(const vk::SwapchainCreateInfoKHR& createInfo) const;
	vk::UniqueImageView								createImageView(const vk::ImageViewCreateInfo& createInfo) const;
	vk::UniqueRenderPass							createRenderPass(const vk::RenderPassCreateInfo& createInfo) const;
	vk::UniqueShaderModule							createShader(const Utils::BufferView<uint32_t>& code) const;
	vk::UniquePipelineLayout						createPipelineLayout(const vk::PipelineLayoutCreateInfo& createInfo) const;
	vk::UniquePipeline								createGraphicsPipeline(	vk::PipelineCache cache,
																			const vk::GraphicsPipelineCreateInfo& createInfo ) const;
	vk::UniqueFramebuffer							createFramebuffer(const vk::FramebufferCreateInfo& createInfo) const;
	vk::UniqueCommandPool							createCommandPool(const vk::CommandPoolCreateInfo& createInfo) const;
	std::vector<vk::UniqueCommandBuffer>			allocateCommnadBuffers(const vk::CommandBufferAllocateInfo& allocInfo) const;
	vk::UniqueSemaphore								createSemaphore() const;
	vk::UniqueFence									createFence(bool signaled = false) const;

private:
	enum QueueIndices {
		GRAPHICS_QUEUE,
		COMPUTE_QUEUE,
		TRANSFER_QUEUE,
		PRESENTATION_QUEUE,
		QUEUE_NUM
	};

	using Samplers = std::array<std::array<vk::UniqueSampler, VK_FILTER_RANGE_SIZE>, SAMPLER_COUNT>;

	vk::DynamicLoader								m_loader;
	vk::DispatchLoaderDynamic						m_dispatcher;
	vk::UniqueInstance								m_instance;
	vk::UniqueDebugUtilsMessengerEXT				m_messenger;
	vk::PhysicalDevice								m_physicalDevice;
	std::array<uint32_t, QUEUE_NUM>					m_queueIndices;
	vk::UniqueDevice								m_device;
	std::array<vk::Queue, QUEUE_NUM>				m_queues;
	FormatSupport									m_formatSupport;
	Samplers										m_samplers;					

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
	static FormatSupport							getFormatSupport(	const vk::DispatchLoaderDynamic& disp, 
																		const vk::PhysicalDevice& physicalDevice );
	static Samplers 								createSamplers(	const vk::DispatchLoaderDynamic& disp, 
																	const vk::Device& device );
																
	static std::vector<vk::LayerProperties> 		getRequiredLayers();
	static std::vector<vk::ExtensionProperties>		getRequiredInstanceExtensions();
	static std::vector<vk::ExtensionProperties>		getRequiredDeviceExtensions();
	static std::vector<vk::QueueFamilyProperties> 	getRequiredQueueFamilies();
};

}