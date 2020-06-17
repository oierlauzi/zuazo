#pragma once

#include "../Version.h"
#include "../Verbosity.h"
#include "../Utils/Pimpl.h"
#include "../Utils/BufferView.h"
#include "../Utils/Area.h"

#include <vector>
#include <array>
#include <utility>

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

	struct AggregatedAllocation {
		vk::UniqueDeviceMemory memory;
		std::vector<Utils::Area> areas;
	};

	static constexpr uint64_t NO_TIMEOUT = std::numeric_limits<uint64_t>::max();

	static constexpr std::array FORMAT_RANGES = {
		std::pair{VK_FORMAT_R4G4_UNORM_PACK8, VK_FORMAT_ASTC_12x12_SRGB_BLOCK }, 			//Normal formats
		std::pair{VK_FORMAT_G8B8G8R8_422_UNORM, VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM }	//YCbCr formats
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
	vk::UniqueDescriptorSetLayout		createDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& createInfo) const;
	vk::DescriptorSetLayout				createDescriptorSetLayout(size_t id) const;
	vk::DescriptorSetLayout				createDescriptorSetLayout(	size_t id,
																	const vk::DescriptorSetLayoutCreateInfo& createInfo ) const;

	vk::UniqueSampler					createSampler(const vk::SamplerCreateInfo& createInfo) const;
	vk::Sampler							createSampler(size_t id) const;
	vk::Sampler							createSampler(	size_t id,
														const vk::SamplerCreateInfo& createInfo ) const;

	std::vector<vk::UniqueCommandBuffer>allocateCommnadBuffers(const vk::CommandBufferAllocateInfo& allocInfo) const;
	vk::UniqueCommandBuffer				allocateCommnadBuffer(const vk::CommandBufferAllocateInfo& allocInfo) const;
	void								resetCommandPool(	vk::CommandPool cmdPool, 
															vk::CommandPoolResetFlags flags ) const;

	vk::UniqueDeviceMemory				allocateMemory(const vk::MemoryAllocateInfo& allocInfo) const;
	vk::UniqueDeviceMemory				allocateMemory(	const vk::MemoryRequirements& requirements,
														vk::MemoryPropertyFlags properties ) const;
	AggregatedAllocation				allocateMemory(	Utils::BufferView<const vk::MemoryRequirements> requirements,
														vk::MemoryPropertyFlags properties ) const;

	vk::MemoryRequirements				getMemoryRequirements(vk::Buffer buf) const;
	vk::MemoryRequirements				getMemoryRequirements(vk::Image img) const;

	void								bindMemory(	vk::Buffer buf, 
													vk::DeviceMemory mem,
													size_t off = 0) const;
	void								bindMemory(	vk::Image img, 
													vk::DeviceMemory mem, 
													size_t off = 0) const;

	std::byte*							mapMemory(const vk::MappedMemoryRange& range) const;
	std::byte*							mapMemory(	vk::DeviceMemory memory, 
													size_t off = 0, 
													size_t size = VK_WHOLE_SIZE ) const;
	void								flushMappedMemory(const vk::MappedMemoryRange& range) const;
	void								flushMappedMemory(	vk::DeviceMemory memory, 
															size_t off = 0, 
															size_t size = VK_WHOLE_SIZE) const;

	void								updateDescriptorSets(	Utils::BufferView<const vk::WriteDescriptorSet> write,
																Utils::BufferView<const vk::CopyDescriptorSet> copy ) const;
	void								updateDescriptorSets(Utils::BufferView<const vk::WriteDescriptorSet> write) const;
	void								updateDescriptorSets(Utils::BufferView<const vk::CopyDescriptorSet> copy) const;

	void								waitIdle() const;
	void								waitForFences(	Utils::BufferView<const vk::Fence> fences,
														bool waitAll = false,
														uint64_t timeout = NO_TIMEOUT) const;
	void								resetFences(Utils::BufferView<const vk::Fence> fences) const;

	void								submit(	vk::Queue queue,
												Utils::BufferView<const vk::SubmitInfo> subInfo,
												vk::Fence fence ) const;

	void								begin(	vk::CommandBuffer cmd,
												const vk::CommandBufferBeginInfo& beginInfo ) const;
	void								end(vk::CommandBuffer cmd) const;

	void								present(vk::SwapchainKHR swapchain,
												uint32_t imageIndex,
												vk::Semaphore waitSemaphore ) const;

	void								presentAll() const;

private:
	struct Impl;
	Utils::Pimpl<Impl>					m_impl;

};

}