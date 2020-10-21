#pragma once

#include "Vulkan.h"

#include <utility>
#include <vector>
#include <memory>

namespace Zuazo::Graphics {

class CommandBuffer {
public:
	using Dependency = std::shared_ptr<void>;

	CommandBuffer(	const Vulkan& vulkan,
					vk::CommandBufferLevel level,
					std::shared_ptr<const vk::UniqueCommandPool> commandPool);
	CommandBuffer(const CommandBuffer& other) = delete;
	CommandBuffer(CommandBuffer&& other) noexcept = default;
	~CommandBuffer() = default;

	CommandBuffer&									operator=(const CommandBuffer& other) = delete;
	CommandBuffer&									operator=(CommandBuffer&& other) noexcept = default;

	const Vulkan&									getVulkan() const noexcept;
	vk::CommandBuffer								getCommandBuffer() const noexcept;

	void											setDependencies(Utils::BufferView<const Dependency> dep);
	Utils::BufferView<const Dependency>				getDependencies() const noexcept;

	void											begin(const vk::CommandBufferBeginInfo& beginInfo) noexcept;
	void											end() noexcept;

	void											execute(Utils::BufferView<const vk::CommandBuffer> buf) noexcept;

	void											pipelineBarrier(vk::PipelineStageFlags srcStageMask,
																	vk::PipelineStageFlags dstStageMask,
																	vk::DependencyFlags dependencyFlags,
																	Utils::BufferView<const vk::MemoryBarrier> memoryBarriers,
																	Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers,
																	Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers ) noexcept;
	void											pipelineBarrier(vk::PipelineStageFlags srcStageMask,
																	vk::PipelineStageFlags dstStageMask,
																	vk::DependencyFlags dependencyFlags,
																	Utils::BufferView<const vk::MemoryBarrier> memoryBarriers ) noexcept;
	void											pipelineBarrier(vk::PipelineStageFlags srcStageMask,
																	vk::PipelineStageFlags dstStageMask,
																	vk::DependencyFlags dependencyFlags,
																	Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers ) noexcept;
	void											pipelineBarrier(vk::PipelineStageFlags srcStageMask,
																	vk::PipelineStageFlags dstStageMask,
																	vk::DependencyFlags dependencyFlags,
																	Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers ) noexcept;

	//Clear commands
	void											clear(	vk::Image image,
															vk::ImageLayout imageLayout,
															const vk::ClearColorValue& value,
															Utils::BufferView<const vk::ImageSubresourceRange> ranges ) noexcept;
	void											clear(	vk::Image image,
															vk::ImageLayout imageLayout,
															const vk::ClearDepthStencilValue& value,
															Utils::BufferView<const vk::ImageSubresourceRange> ranges ) noexcept;
	void											clear(	Utils::BufferView<const vk::ClearAttachment> attachments,
															Utils::BufferView<const vk::ClearRect> rects ) noexcept;
	void											clear(	vk::Buffer buffer,
															const Utils::Area& area,
															uint32_t data = 0 ) noexcept;
	void											clear(	vk::Buffer buffer,
															const Utils::Area& area,
															const std::byte* data ) noexcept;

	//Transfer commands
	void											copy(	vk::Buffer srcBuffer,
															vk::Buffer dstBuffer,
															Utils::BufferView<const vk::BufferCopy> regions ) noexcept;
	void 											copy(	vk::Image srcImage,
    														vk::ImageLayout srcImageLayout,
    														vk::Image dstImage,
    														vk::ImageLayout dstImageLayout,
															Utils::BufferView<const vk::ImageCopy> regions ) noexcept;
	void 											copy(	vk::Buffer srcBuffer,
    														vk::Image dstImage,
    														vk::ImageLayout dstImageLayout,
															Utils::BufferView<const vk::BufferImageCopy> regions ) noexcept;
	void 											copy(	vk::Image srcImage,
    														vk::ImageLayout srcImageLayout,
    														vk::Buffer dstBuffer,
															Utils::BufferView<const vk::BufferImageCopy> regions ) noexcept;
	void 											blit(	vk::Image srcImage,
    														vk::ImageLayout srcImageLayout,
    														vk::Image dstImage,
    														vk::ImageLayout dstImageLayout,
															Utils::BufferView<const vk::ImageBlit> regions,
    														vk::Filter filter ) noexcept;
	void 											resolve(vk::Image srcImage,
    														vk::ImageLayout srcImageLayout,
    														vk::Image dstImage,
    														vk::ImageLayout dstImageLayout,
															Utils::BufferView<const vk::ImageResolve> regions ) noexcept;

	//Draw commands
	void											beginRenderPass(const vk::RenderPassBeginInfo& beginInfo,
																	vk::SubpassContents contents ) noexcept;
	void											endRenderPass() noexcept;
	void											nextSubpass(vk::SubpassContents contents) noexcept;
	void											bindPipeline(	vk::PipelineBindPoint bindPoint, 
																	vk::Pipeline pipeline ) noexcept;
	void											bindVertexBuffers(	uint32_t firstBinding, 
																		Utils::BufferView<const vk::Buffer> buffers, 
																		Utils::BufferView<const vk::DeviceSize> offsets ) noexcept;
	void											bindIndexBuffer(vk::Buffer buffer,
   																	size_t offset,
    																vk::IndexType indexType ) noexcept;
	void											bindDescriptorSets(	vk::PipelineBindPoint pipelineBindPoint, 
																		vk::PipelineLayout layout, 
																		uint32_t firstSet, 
																		Utils::BufferView<const vk::DescriptorSet> descriptorSets, 
																		Utils::BufferView<const uint32_t> dynamicOffsets) noexcept;
	void											draw(	uint32_t vertexCount, 
															uint32_t instanceCount, 
															uint32_t firstVertex, 
															uint32_t firstInstance ) noexcept;
	void											draw(	vk::Buffer buffer,
    														size_t offset,
    														uint32_t drawCount,
    														uint32_t stride ) noexcept;
	void											drawIndexed(uint32_t indexCount,
    															uint32_t instanceCount,
    															uint32_t firstIndex,
    															int32_t vertexOffset,
    															uint32_t firstInstance ) noexcept;
	void											drawIndexed(vk::Buffer buffer,
    															size_t offset,
    															uint32_t drawCount,
    															uint32_t stride ) noexcept;
private:
	std::reference_wrapper<const Vulkan>			m_vulkan;
	std::shared_ptr<const vk::UniqueCommandPool>	m_commandPool;
	vk::UniqueCommandBuffer							m_commandBuffer;

	std::vector<Dependency>							m_dependencies;

	static vk::UniqueCommandBuffer					createCommandBuffer(const Vulkan& vulkan,
																		vk::CommandBufferLevel level,
																		vk::CommandPool pool);

};

}