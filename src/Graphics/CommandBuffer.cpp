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



const Vulkan& CommandBuffer::getVulkan() const noexcept {
	return m_vulkan;
}

vk::CommandBuffer CommandBuffer::getCommandBuffer() const noexcept {
	return *m_commandBuffer;
}


void CommandBuffer::clearDependencies() noexcept {
	m_dependencies.clear();
}

void CommandBuffer::addDependencies(Utils::BufferView<const Dependency> dep) {
	m_dependencies.insert(m_dependencies.cend(), dep.cbegin(), dep.cend());
}

Utils::BufferView<const CommandBuffer::Dependency> CommandBuffer::getDependencies() const noexcept {
	return Utils::BufferView<const Dependency>(m_dependencies.data(), m_dependencies.data() + m_dependencies.size());
}



void CommandBuffer::begin(const vk::CommandBufferBeginInfo& beginInfo) noexcept {
	getVulkan().begin(getCommandBuffer(), beginInfo);
}

void CommandBuffer::end() noexcept {
	getVulkan().end(getCommandBuffer());
}



void CommandBuffer::execute(Utils::BufferView<const vk::CommandBuffer> buf) noexcept {
	getVulkan().execute(getCommandBuffer(), buf);
}



void CommandBuffer::pipelineBarrier(vk::PipelineStageFlags srcStageMask,
									vk::PipelineStageFlags dstStageMask,
									vk::DependencyFlags dependencyFlags,
									Utils::BufferView<const vk::MemoryBarrier> memoryBarriers,
									Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers,
									Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers ) noexcept
{
	getVulkan().pipelineBarrier(getCommandBuffer(), srcStageMask, dstStageMask, dependencyFlags, memoryBarriers, bufferMemoryBarriers, imageMemoryBarriers);
}

void CommandBuffer::pipelineBarrier(vk::PipelineStageFlags srcStageMask,
									vk::PipelineStageFlags dstStageMask,
									vk::DependencyFlags dependencyFlags,
									Utils::BufferView<const vk::MemoryBarrier> memoryBarriers ) noexcept
{
	getVulkan().pipelineBarrier(getCommandBuffer(), srcStageMask, dstStageMask, dependencyFlags, memoryBarriers);
}

void CommandBuffer::pipelineBarrier(vk::PipelineStageFlags srcStageMask,
									vk::PipelineStageFlags dstStageMask,
									vk::DependencyFlags dependencyFlags,
									Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers ) noexcept
{
	getVulkan().pipelineBarrier(getCommandBuffer(), srcStageMask, dstStageMask, dependencyFlags, bufferMemoryBarriers);
}

void CommandBuffer::pipelineBarrier(vk::PipelineStageFlags srcStageMask,
									vk::PipelineStageFlags dstStageMask,
									vk::DependencyFlags dependencyFlags,
									Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers ) noexcept
{
	getVulkan().pipelineBarrier(getCommandBuffer(), srcStageMask, dstStageMask, dependencyFlags, imageMemoryBarriers);
}



void CommandBuffer::clear(	vk::Image image,
							vk::ImageLayout imageLayout,
							const vk::ClearColorValue& value,
							Utils::BufferView<const vk::ImageSubresourceRange> ranges ) noexcept
{
	getVulkan().clear(getCommandBuffer(), image, imageLayout, value, ranges);
}

void CommandBuffer::clear(	vk::Image image,
							vk::ImageLayout imageLayout,
							const vk::ClearDepthStencilValue& value,
							Utils::BufferView<const vk::ImageSubresourceRange> ranges ) noexcept
{
	getVulkan().clear(getCommandBuffer(), image, imageLayout, value, ranges);
}

void CommandBuffer::clear(	Utils::BufferView<const vk::ClearAttachment> attachments,
							Utils::BufferView<const vk::ClearRect> rects ) noexcept
{
	getVulkan().clear(getCommandBuffer(), attachments, rects);
}

void CommandBuffer::clear(	vk::Buffer buffer,
							const Utils::Area& area,
							uint32_t data ) noexcept
{
	getVulkan().clear(getCommandBuffer(), buffer, area, data);
}

void CommandBuffer::clear(	vk::Buffer buffer,
							const Utils::Area& area,
							const std::byte* data ) noexcept
{
	getVulkan().clear(getCommandBuffer(), buffer, area, data);
}



void CommandBuffer::copy(	vk::Buffer srcBuffer,
							vk::Buffer dstBuffer,
							Utils::BufferView<const vk::BufferCopy> regions ) noexcept
{
	getVulkan().copy(getCommandBuffer(), srcBuffer, dstBuffer, regions);
}

void CommandBuffer::copy(	vk::Image srcImage,
							vk::ImageLayout srcImageLayout,
							vk::Image dstImage,
							vk::ImageLayout dstImageLayout,
							Utils::BufferView<const vk::ImageCopy> regions ) noexcept
{
	getVulkan().copy(getCommandBuffer(), srcImage, srcImageLayout, dstImage, dstImageLayout, regions);
}

void CommandBuffer::copy(	vk::Buffer srcBuffer,
							vk::Image dstImage,
							vk::ImageLayout dstImageLayout,
							Utils::BufferView<const vk::BufferImageCopy> regions ) noexcept
{
	getVulkan().copy(getCommandBuffer(), srcBuffer, dstImage, dstImageLayout, regions);
}

void CommandBuffer::copy(	vk::Image srcImage,
							vk::ImageLayout srcImageLayout,
							vk::Buffer dstBuffer,
							Utils::BufferView<const vk::BufferImageCopy> regions ) noexcept
{
	getVulkan().copy(getCommandBuffer(), srcImage, srcImageLayout, dstBuffer, regions);
}

void CommandBuffer::blit(	vk::Image srcImage,
							vk::ImageLayout srcImageLayout,
							vk::Image dstImage,
							vk::ImageLayout dstImageLayout,
							Utils::BufferView<const vk::ImageBlit> regions,
							vk::Filter filter ) noexcept
{
	getVulkan().blit(getCommandBuffer(), srcImage, srcImageLayout, dstImage, dstImageLayout, regions, filter);
}

void CommandBuffer::resolve(vk::Image srcImage,
							vk::ImageLayout srcImageLayout,
							vk::Image dstImage,
							vk::ImageLayout dstImageLayout,
							Utils::BufferView<const vk::ImageResolve> regions ) noexcept
{
	getVulkan().resolve(getCommandBuffer(), srcImage, srcImageLayout, dstImage, dstImageLayout, regions);
}



void CommandBuffer::beginRenderPass(const vk::RenderPassBeginInfo& beginInfo,
									vk::SubpassContents contents ) noexcept
{
	getVulkan().beginRenderPass(getCommandBuffer(), beginInfo, contents);
}

void CommandBuffer::endRenderPass() noexcept {
	getVulkan().endRenderPass(getCommandBuffer());
}

void CommandBuffer::nextSubpass(vk::SubpassContents contents) noexcept {
		getVulkan().nextSubpass(getCommandBuffer(), contents);
}

void CommandBuffer::bindPipeline(	vk::PipelineBindPoint bindPoint, 
									vk::Pipeline pipeline ) noexcept
{
	getVulkan().bindPipeline(getCommandBuffer(), bindPoint, pipeline);
}

void CommandBuffer::bindVertexBuffers(	uint32_t firstBinding, 
										Utils::BufferView<const vk::Buffer> buffers, 
										Utils::BufferView<const vk::DeviceSize> offsets ) noexcept
{
	getVulkan().bindVertexBuffers(getCommandBuffer(), firstBinding, buffers, offsets);
}

void CommandBuffer::bindDescriptorSets(	vk::PipelineBindPoint pipelineBindPoint, 
										vk::PipelineLayout layout, 
										uint32_t firstSet, 
										Utils::BufferView<const vk::DescriptorSet> descriptorSets, 
										Utils::BufferView<const uint32_t> dynamicOffsets ) noexcept
{
	getVulkan().bindDescriptorSets(getCommandBuffer(), pipelineBindPoint, layout, firstSet, descriptorSets, dynamicOffsets);
}

void CommandBuffer::setViewport(uint32_t first,
								Utils::BufferView<const vk::Viewport> viewports ) noexcept 
{
	getVulkan().setViewport(getCommandBuffer(), first, viewports);
}

void CommandBuffer::setScissor(	uint32_t first,
								Utils::BufferView<const vk::Rect2D> scissors ) noexcept
{
	getVulkan().setScissor(getCommandBuffer(), first, scissors);
}

void CommandBuffer::draw(	uint32_t vertexCount, 
							uint32_t instanceCount, 
							uint32_t firstVertex, 
							uint32_t firstInstance ) noexcept
{
	getVulkan().draw(getCommandBuffer(), vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::draw(	vk::Buffer buffer,
							size_t offset,
							uint32_t drawCount,
							uint32_t stride ) noexcept
{
	getVulkan().draw(getCommandBuffer(), buffer, offset, drawCount, stride);
}

void CommandBuffer::drawIndexed(uint32_t indexCount,
								uint32_t instanceCount,
								uint32_t firstIndex,
								int32_t vertexOffset,
								uint32_t firstInstance ) noexcept
{
	getVulkan().drawIndexed(getCommandBuffer(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::drawIndexed(vk::Buffer buffer,
								size_t offset,
								uint32_t drawCount,
								uint32_t stride ) noexcept
{
	getVulkan().drawIndexed(getCommandBuffer(), buffer, offset, drawCount, stride);
}



vk::UniqueCommandBuffer CommandBuffer::createCommandBuffer(	const Vulkan& vulkan,
															vk::CommandBufferLevel level,
															vk::CommandPool pool )
{
	return vulkan.allocateCommnadBuffer(pool, level);
}

}