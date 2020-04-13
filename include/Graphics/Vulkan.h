#pragma once

#include "../Version.h"
#include "../Verbosity.h"
#include "../Utils/Pimpl.h"
#include "../Utils/BufferView.h"

#include <vector>
#include <array>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_DEFAULT_DISPATCHER void()
#include <vulkan/vulkan.hpp>

namespace Zuazo::Graphics {

class Vulkan {
public:
	using DeviceScoreFunc = std::function<uint32_t(const vk::DispatchLoaderDynamic&, vk::PhysicalDevice)>;

	struct FormatSupport {
		std::vector<vk::Format>	sampler;
		std::vector<vk::Format>	framebuffer;
	};

	Vulkan(	std::string_view appName, 
			Version appVersion,
			const DeviceScoreFunc& scoreFunc );
	Vulkan(const Vulkan& other) = delete;
	Vulkan(Vulkan&& other);
	~Vulkan();

	Vulkan&								operator=(const Vulkan& other) = delete;
	Vulkan&								operator=(Vulkan&& other);

	const vk::DynamicLoader&			getLoader() const;
	const vk::DispatchLoaderDynamic&	getDispatcher() const;
	vk::Instance						getInstance() const;
	vk::PhysicalDevice					getPhysicalDevice() const;
	vk::Device							getDevice() const;
	uint32_t							getGraphicsQueueIndex() const;
	vk::Queue							getGraphicsQueue() const;
	uint32_t							getComputeQueueIndex() const;
	vk::Queue							getComputeQueue() const;
	uint32_t							getTransferQueueIndex() const;
	vk::Queue							getTransferQueue() const;
	uint32_t							getPresentationQueueIndex() const;
	vk::Queue							getPresentationQueue() const;
	vk::PipelineCache					getPipelineCache() const;
	vk::Sampler							getSampler(vk::Filter filter) const;
	const FormatSupport&				getFormatSupport() const;
	
	vk::FormatProperties				getFormatFeatures(vk::Format format) const;

	vk::UniqueSwapchainKHR				createSwapchain(const vk::SwapchainCreateInfoKHR& createInfo) const;
	vk::UniqueImage						createImage(const vk::ImageCreateInfo& createInfo) const;
	vk::UniqueImageView					createImageView(const vk::ImageViewCreateInfo& createInfo) const;
	vk::UniqueRenderPass				createRenderPass(const vk::RenderPassCreateInfo& createInfo) const;
	vk::UniquePipeline					createGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& createInfo ) const;
	vk::UniqueFramebuffer				createFramebuffer(const vk::FramebufferCreateInfo& createInfo) const;
	vk::UniqueCommandPool				createCommandPool(const vk::CommandPoolCreateInfo& createInfo) const;
	vk::UniqueBuffer					createBuffer(const vk::BufferCreateInfo& createInfo) const;
	vk::UniqueDescriptorPool			createDescriptorPool(const vk::DescriptorPoolCreateInfo& createInfo) const;
	vk::UniqueSemaphore					createSemaphore() const;
	vk::UniqueFence						createFence(bool signaled = false) const;

	vk::UniqueShaderModule				createShaderModule(Utils::BufferView<const uint32_t> code) const;
	vk::ShaderModule					createShaderModule(size_t id) const;
	vk::ShaderModule					createShaderModule(	size_t id, 
															Utils::BufferView<const uint32_t> code ) const;
	vk::UniquePipelineLayout			createPipelineLayout(const vk::PipelineLayoutCreateInfo& createInfo) const;
	vk::PipelineLayout					createPipelineLayout(size_t id) const;
	vk::PipelineLayout					createPipelineLayout(	size_t id,
																const vk::PipelineLayoutCreateInfo& createInfo ) const;
	vk::UniqueDescriptorSetLayout		createDescriptorSetLayout(	const vk::DescriptorSetLayoutCreateInfo& createInfo) const;
	vk::DescriptorSetLayout				createDescriptorSetLayout(size_t id) const;
	vk::DescriptorSetLayout				createDescriptorSetLayout(	size_t id,
																	const vk::DescriptorSetLayoutCreateInfo& createInfo ) const;

	std::vector<vk::UniqueCommandBuffer>allocateCommnadBuffers(const vk::CommandBufferAllocateInfo& allocInfo) const;
	vk::UniqueDeviceMemory				allocateMemory(const vk::MemoryAllocateInfo& allocInfo) const;
	vk::UniqueDeviceMemory				allocateMemory(	const vk::MemoryRequirements& requirements,
														vk::MemoryPropertyFlags properties ) const;

	std::byte*							mapMemory(const vk::MappedMemoryRange& range) const;
	void								flushMappedMemory(const vk::MappedMemoryRange& range) const;

	void								present(vk::SwapchainKHR swapchain,
												uint32_t imageIndex,
												vk::Semaphore waitSemaphore ) const;

	void								presentAll() const;

private:
	struct Impl;
	Utils::Pimpl<Impl>					m_impl;

};

}