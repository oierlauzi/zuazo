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
	void											beginRenderPass(const vk::RenderPassBeginInfo& beginInfo,
																	vk::SubpassContents contents );
	void											endRenderPass();
	void											bindPipeline(	vk::PipelineBindPoint bindPoint, 
																	vk::Pipeline pipeline );
	void											bindVertexBuffers(	uint32_t firstBinding, 
																		Utils::BufferView<const vk::Buffer> buffers, 
																		Utils::BufferView<const vk::DeviceSize> offsets );
	void											bindDescriptorSets(	vk::PipelineBindPoint pipelineBindPoint, 
																		vk::PipelineLayout layout, 
																		uint32_t firstSet, 
																		Utils::BufferView<const vk::DescriptorSet> descriptorSets, 
																		Utils::BufferView<const uint32_t> dynamicOffsets);
	void											draw(	uint32_t vertexCount, 
															uint32_t instanceCount, 
															uint32_t firstVertex, 
															uint32_t firstInstance );

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