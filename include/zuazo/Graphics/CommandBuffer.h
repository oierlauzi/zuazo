#pragma once

#include "Vulkan.h"

#include <utility>
#include <vector>
#include <memory>

namespace Zuazo::Graphics {

class CommandBuffer {
public:
	using Dependencies = std::vector<std::shared_ptr<void>>;

	CommandBuffer(	const Vulkan& vulkan,
					vk::CommandBufferLevel level,
					std::shared_ptr<const vk::UniqueCommandPool> commandPool);
	CommandBuffer(const CommandBuffer& other) = delete;
	CommandBuffer(CommandBuffer&& other) = default;
	~CommandBuffer() = default;

	CommandBuffer&									operator=(const CommandBuffer& other) = delete;
	CommandBuffer&									operator=(CommandBuffer&& other) = default;

	const Vulkan&									getVulkan() const;
	vk::CommandBuffer								getCommandBuffer() const;

	void											setDependencies(Dependencies dep);
	Dependencies									releaseDependencies();
	const Dependencies&								getDependencies() const;

	void											begin(const vk::CommandBufferBeginInfo& beginInfo);
	void											end();

	void											execute(Utils::BufferView<const vk::CommandBuffer> buf);

	void											pipelineBarrier(vk::PipelineStageFlags srcStageMask,
																	vk::PipelineStageFlags dstStageMask,
																	vk::DependencyFlags dependencyFlags,
																	Utils::BufferView<const vk::MemoryBarrier> memoryBarriers,
																	Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers,
																	Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers );
	void											pipelineBarrier(vk::PipelineStageFlags srcStageMask,
																	vk::PipelineStageFlags dstStageMask,
																	vk::DependencyFlags dependencyFlags,
																	Utils::BufferView<const vk::MemoryBarrier> memoryBarriers );
	void											pipelineBarrier(vk::PipelineStageFlags srcStageMask,
																	vk::PipelineStageFlags dstStageMask,
																	vk::DependencyFlags dependencyFlags,
																	Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers );
	void											pipelineBarrier(vk::PipelineStageFlags srcStageMask,
																	vk::PipelineStageFlags dstStageMask,
																	vk::DependencyFlags dependencyFlags,
																	Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers );

	//Clear commands
	void											clear(	vk::Image image,
															vk::ImageLayout imageLayout,
															const vk::ClearColorValue& value,
															Utils::BufferView<const vk::ImageSubresourceRange> ranges );
	void											clear(	vk::Image image,
															vk::ImageLayout imageLayout,
															const vk::ClearDepthStencilValue& value,
															Utils::BufferView<const vk::ImageSubresourceRange> ranges );
	void											clear(	Utils::BufferView<const vk::ClearAttachment> attachments,
															Utils::BufferView<const vk::ClearRect> rects );
	void											clear(	vk::Buffer buffer,
															const Utils::Area& area,
															uint32_t data = 0 );
	void											clear(	vk::Buffer buffer,
															const Utils::Area& area,
															const std::byte* data );

	//Transfer commands
	void											copy(	vk::Buffer srcBuffer,
															vk::Buffer dstBuffer,
															Utils::BufferView<const vk::BufferCopy> regions );
	void 											copy(	vk::Image srcImage,
    														vk::ImageLayout srcImageLayout,
    														vk::Image dstImage,
    														vk::ImageLayout dstImageLayout,
															Utils::BufferView<const vk::ImageCopy> regions );
	void 											copy(	vk::Buffer srcBuffer,
    														vk::Image dstImage,
    														vk::ImageLayout dstImageLayout,
															Utils::BufferView<const vk::BufferImageCopy> regions );
	void 											copy(	vk::Image srcImage,
    														vk::ImageLayout srcImageLayout,
    														vk::Buffer dstBuffer,
															Utils::BufferView<const vk::BufferImageCopy> regions );
	void 											blit(	vk::Image srcImage,
    														vk::ImageLayout srcImageLayout,
    														vk::Image dstImage,
    														vk::ImageLayout dstImageLayout,
															Utils::BufferView<const vk::ImageBlit> regions,
    														vk::Filter filter );
	void 											resolve(vk::Image srcImage,
    														vk::ImageLayout srcImageLayout,
    														vk::Image dstImage,
    														vk::ImageLayout dstImageLayout,
															Utils::BufferView<const vk::ImageResolve> regions );

	//Draw commands
	void											beginRenderPass(const vk::RenderPassBeginInfo& beginInfo,
																	vk::SubpassContents contents );
	void											endRenderPass();
	void											nextSubpass(vk::SubpassContents contents);
	void											bindPipeline(	vk::PipelineBindPoint bindPoint, 
																	vk::Pipeline pipeline );
	void											bindVertexBuffers(	uint32_t firstBinding, 
																		Utils::BufferView<const vk::Buffer> buffers, 
																		Utils::BufferView<const vk::DeviceSize> offsets );
	void											bindIndexBuffer(vk::Buffer buffer,
   																	size_t offset,
    																vk::IndexType indexType );
	void											bindDescriptorSets(	vk::PipelineBindPoint pipelineBindPoint, 
																		vk::PipelineLayout layout, 
																		uint32_t firstSet, 
																		Utils::BufferView<const vk::DescriptorSet> descriptorSets, 
																		Utils::BufferView<const uint32_t> dynamicOffsets);
	void											draw(	uint32_t vertexCount, 
															uint32_t instanceCount, 
															uint32_t firstVertex, 
															uint32_t firstInstance );
	void											draw(	vk::Buffer buffer,
    														size_t offset,
    														uint32_t drawCount,
    														uint32_t stride );
	void											drawIndexed(uint32_t indexCount,
    															uint32_t instanceCount,
    															uint32_t firstIndex,
    															int32_t vertexOffset,
    															uint32_t firstInstance );
	void											drawIndexed(vk::Buffer buffer,
    															size_t offset,
    															uint32_t drawCount,
    															uint32_t stride );
private:
	std::reference_wrapper<const Vulkan>			m_vulkan;
	std::shared_ptr<const vk::UniqueCommandPool>	m_commandPool;
	vk::UniqueCommandBuffer							m_commandBuffer;

	Dependencies									m_dependencies;

	static vk::UniqueCommandBuffer					createCommandBuffer(const Vulkan& vulkan,
																		vk::CommandBufferLevel level,
																		vk::CommandPool pool);

};

}