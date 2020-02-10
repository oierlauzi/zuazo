#include <Graphics/CommandPool.h>

namespace Zuazo::Graphics {

CommandPool::CommandPool(	const Vulkan& vulkan,
							uint32_t familyIndex,
							uint32_t bufferCount )
	: m_commandPool(createCommandPool(	
		vulkan, 
		familyIndex ))
	, m_commandBuffers(createCommandBuffers(
		vulkan,
		*m_commandPool,
		bufferCount ))
{
}

vk::CommandPool	CommandPool::getCommandPool() const{
	return *m_commandPool;
}
const std::vector<vk::UniqueCommandBuffer>& CommandPool::getCommandBuffers() const{
	return m_commandBuffers;
}



vk::UniqueCommandPool CommandPool::createCommandPool(	const Vulkan& vulkan,
														uint32_t familyIndex )
{
	const vk::CommandPoolCreateInfo createInfo(
		{}, 
		familyIndex
	);

	return vulkan.getDevice().createCommandPoolUnique(createInfo, nullptr, vulkan.getDispatcher());
}

std::vector<vk::UniqueCommandBuffer> CommandPool::createCommandBuffers(	const Vulkan& vulkan,
																		vk::CommandPool pool,
	 																	uint32_t bufferCount )
{
	const vk::CommandBufferAllocateInfo allocInfo(
		pool,
		vk::CommandBufferLevel::ePrimary,
        bufferCount
	);

	return vulkan.getDevice().allocateCommandBuffersUnique(allocInfo, vulkan.getDispatcher());
}

}