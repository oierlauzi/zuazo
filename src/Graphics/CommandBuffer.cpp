#include <zuazo/Graphics/CommandBuffer.h>

#include <cassert>

namespace Zuazo::Graphics {

CommandBuffer::CommandBuffer(	const Vulkan& vulkan,
								vk::CommandBufferLevel level,
								std::shared_ptr<const vk::UniqueCommandPool> commandPool )
	: m_vulkan(vulkan)
	, m_commandPool(std::move(commandPool))
	, m_commandBuffer(createCommandBuffer(m_vulkan, level, **m_commandPool))
{
}



const Vulkan& CommandBuffer::getVulkan() const {
	return m_vulkan;
}

vk::CommandBuffer CommandBuffer::getCommandBuffer() const {
	return *m_commandBuffer;
}



void CommandBuffer::setDependencies(Dependencies dep) {
	m_dependencies = std::move(dep);
}

CommandBuffer::Dependencies CommandBuffer::releaseDependencies() {
	return std::move(m_dependencies);
}

const CommandBuffer::Dependencies& CommandBuffer::getDependencies() const {
	return m_dependencies;
}



void CommandBuffer::begin(const vk::CommandBufferBeginInfo& beginInfo) {
	getVulkan().begin(getCommandBuffer(), beginInfo);
}

void CommandBuffer::end() {
	getVulkan().end(getCommandBuffer());
}

void CommandBuffer::beginRenderPass(const vk::RenderPassBeginInfo& beginInfo,
									vk::SubpassContents contents )
{
	getVulkan().beginRenderPass(getCommandBuffer(), beginInfo, contents);
}

void CommandBuffer::CommandBuffer::endRenderPass() {
	getVulkan().endRenderPass(getCommandBuffer());
}

void CommandBuffer::bindPipeline(	vk::PipelineBindPoint bindPoint, 
									vk::Pipeline pipeline )
{
	getVulkan().bindPipeline(getCommandBuffer(), bindPoint, pipeline);
}

void CommandBuffer::bindVertexBuffers(	uint32_t firstBinding, 
										Utils::BufferView<const vk::Buffer> buffers, 
										Utils::BufferView<const vk::DeviceSize> offsets )
{
	getVulkan().bindVertexBuffers(getCommandBuffer(), firstBinding, buffers, offsets);
}

void CommandBuffer::bindDescriptorSets(	vk::PipelineBindPoint pipelineBindPoint, 
										vk::PipelineLayout layout, 
										uint32_t firstSet, 
										Utils::BufferView<const vk::DescriptorSet> descriptorSets, 
										Utils::BufferView<const uint32_t> dynamicOffsets )
{
	getVulkan().bindDescriptorSets(getCommandBuffer(), pipelineBindPoint, layout, firstSet, descriptorSets, dynamicOffsets);
}

void CommandBuffer::draw(	uint32_t vertexCount, 
							uint32_t instanceCount, 
							uint32_t firstVertex, 
							uint32_t firstInstance )
{
	getVulkan().draw(getCommandBuffer(), vertexCount, instanceCount, firstVertex, firstInstance);
}





vk::UniqueCommandBuffer CommandBuffer::createCommandBuffer(	const Vulkan& vulkan,
															vk::CommandBufferLevel level,
															vk::CommandPool pool )
{
	assert(pool);
	const vk::CommandBufferAllocateInfo allocInfo(
		pool,
		level,
		1
	);

	return vulkan.allocateCommnadBuffer(allocInfo);
}

}