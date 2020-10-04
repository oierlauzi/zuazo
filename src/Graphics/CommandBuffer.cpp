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



void CommandBuffer::execute(Utils::BufferView<const vk::CommandBuffer> buf) {
	getVulkan().execute(getCommandBuffer(), buf);
}



void CommandBuffer::pipelineBarrier(vk::PipelineStageFlags srcStageMask,
									vk::PipelineStageFlags dstStageMask,
									vk::DependencyFlags dependencyFlags,
									Utils::BufferView<const vk::MemoryBarrier> memoryBarriers,
									Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers,
									Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers )
{
	getVulkan().pipelineBarrier(getCommandBuffer(), srcStageMask, dstStageMask, dependencyFlags, memoryBarriers, bufferMemoryBarriers, imageMemoryBarriers);
}

void CommandBuffer::pipelineBarrier(vk::PipelineStageFlags srcStageMask,
									vk::PipelineStageFlags dstStageMask,
									vk::DependencyFlags dependencyFlags,
									Utils::BufferView<const vk::MemoryBarrier> memoryBarriers )
{
	getVulkan().pipelineBarrier(getCommandBuffer(), srcStageMask, dstStageMask, dependencyFlags, memoryBarriers);
}

void CommandBuffer::pipelineBarrier(vk::PipelineStageFlags srcStageMask,
									vk::PipelineStageFlags dstStageMask,
									vk::DependencyFlags dependencyFlags,
									Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers )
{
	getVulkan().pipelineBarrier(getCommandBuffer(), srcStageMask, dstStageMask, dependencyFlags, bufferMemoryBarriers);
}

void CommandBuffer::pipelineBarrier(vk::PipelineStageFlags srcStageMask,
									vk::PipelineStageFlags dstStageMask,
									vk::DependencyFlags dependencyFlags,
									Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers )
{
	getVulkan().pipelineBarrier(getCommandBuffer(), srcStageMask, dstStageMask, dependencyFlags, imageMemoryBarriers);
}



void CommandBuffer::clear(	vk::Image image,
							vk::ImageLayout imageLayout,
							const vk::ClearColorValue& value,
							Utils::BufferView<const vk::ImageSubresourceRange> ranges )
{
	getVulkan().clear(getCommandBuffer(), image, imageLayout, value, ranges);
}

void CommandBuffer::clear(	vk::Image image,
							vk::ImageLayout imageLayout,
							const vk::ClearDepthStencilValue& value,
							Utils::BufferView<const vk::ImageSubresourceRange> ranges )
{
	getVulkan().clear(getCommandBuffer(), image, imageLayout, value, ranges);
}

void CommandBuffer::clear(	Utils::BufferView<const vk::ClearAttachment> attachments,
							Utils::BufferView<const vk::ClearRect> rects )
{
	getVulkan().clear(getCommandBuffer(), attachments, rects);
}

void CommandBuffer::clear(	vk::Buffer buffer,
							const Utils::Area& area,
							uint32_t data )
{
	getVulkan().clear(getCommandBuffer(), buffer, area, data);
}

void CommandBuffer::clear(	vk::Buffer buffer,
							const Utils::Area& area,
							const std::byte* data )
{
	getVulkan().clear(getCommandBuffer(), buffer, area, data);
}



void CommandBuffer::copy(	vk::Buffer srcBuffer,
							vk::Buffer dstBuffer,
							Utils::BufferView<const vk::BufferCopy> regions )
{
	getVulkan().copy(getCommandBuffer(), srcBuffer, dstBuffer, regions);
}

void CommandBuffer::copy(	vk::Image srcImage,
							vk::ImageLayout srcImageLayout,
							vk::Image dstImage,
							vk::ImageLayout dstImageLayout,
							Utils::BufferView<const vk::ImageCopy> regions )
{
	getVulkan().copy(getCommandBuffer(), srcImage, srcImageLayout, dstImage, dstImageLayout, regions);
}

void CommandBuffer::copy(	vk::Buffer srcBuffer,
							vk::Image dstImage,
							vk::ImageLayout dstImageLayout,
							Utils::BufferView<const vk::BufferImageCopy> regions )
{
	getVulkan().copy(getCommandBuffer(), srcBuffer, dstImage, dstImageLayout, regions);
}

void CommandBuffer::copy(	vk::Image srcImage,
							vk::ImageLayout srcImageLayout,
							vk::Buffer dstBuffer,
							Utils::BufferView<const vk::BufferImageCopy> regions )
{
	getVulkan().copy(getCommandBuffer(), srcImage, srcImageLayout, dstBuffer, regions);
}

void CommandBuffer::blit(	vk::Image srcImage,
							vk::ImageLayout srcImageLayout,
							vk::Image dstImage,
							vk::ImageLayout dstImageLayout,
							Utils::BufferView<const vk::ImageBlit> regions,
							vk::Filter filter )
{
	getVulkan().blit(getCommandBuffer(), srcImage, srcImageLayout, dstImage, dstImageLayout, regions, filter);
}

void CommandBuffer::resolve(vk::Image srcImage,
							vk::ImageLayout srcImageLayout,
							vk::Image dstImage,
							vk::ImageLayout dstImageLayout,
							Utils::BufferView<const vk::ImageResolve> regions )
{
	getVulkan().resolve(getCommandBuffer(), srcImage, srcImageLayout, dstImage, dstImageLayout, regions);
}



void CommandBuffer::beginRenderPass(const vk::RenderPassBeginInfo& beginInfo,
									vk::SubpassContents contents )
{
	getVulkan().beginRenderPass(getCommandBuffer(), beginInfo, contents);
}

void CommandBuffer::endRenderPass() {
	getVulkan().endRenderPass(getCommandBuffer());
}

void CommandBuffer::nextSubpass(vk::SubpassContents contents) {
		getVulkan().nextSubpass(getCommandBuffer(), contents);
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

void CommandBuffer::draw(	vk::Buffer buffer,
							size_t offset,
							uint32_t drawCount,
							uint32_t stride )
{
	getVulkan().draw(getCommandBuffer(), buffer, offset, drawCount, stride);
}

void CommandBuffer::drawIndexed(uint32_t indexCount,
								uint32_t instanceCount,
								uint32_t firstIndex,
								int32_t vertexOffset,
								uint32_t firstInstance )
{
	getVulkan().drawIndexed(getCommandBuffer(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::drawIndexed(vk::Buffer buffer,
								size_t offset,
								uint32_t drawCount,
								uint32_t stride )
{
	getVulkan().drawIndexed(getCommandBuffer(), buffer, offset, drawCount, stride);
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