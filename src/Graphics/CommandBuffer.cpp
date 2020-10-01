#include <zuazo/Graphics/CommandBuffer.h>

#include <cassert>

namespace Zuazo::Graphics {

CommandBuffer::CommandBuffer(	const Vulkan& vulkan,
								vk::CommandBufferLevel level,
								std::shared_ptr<const vk::UniqueCommandPool> commandPool )
	: m_vulkan(vulkan)
	, m_commandPool(std::move(commandPool))
	, m_commandBuffer(createCommandBuffer(m_vulkan, level, m_commandPool))
{
}



const Vulkan& CommandBuffer::getVulkan() const {
	return m_vulkan;
}

vk::CommandBuffer CommandBuffer::getCommandBuffer() const {
	return *m_commandBuffer;
}



vk::UniqueCommandBuffer CommandBuffer::createCommandBuffer(	const Vulkan& vulkan,
															vk::CommandBufferLevel level,
															const std::shared_ptr<const vk::UniqueCommandPool>& pool )
{
	assert(pool);
	assert(*pool);

	const vk::CommandBufferAllocateInfo allocInfo(
		**pool,
		level,
		1
	);

	return vulkan.allocateCommnadBuffer(allocInfo);
}

}