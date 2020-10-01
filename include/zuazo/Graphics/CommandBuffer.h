#pragma once

#include "Vulkan.h"

#include <utility>

namespace Zuazo::Graphics {

class CommandBuffer {
public:
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

private:
	std::reference_wrapper<const Vulkan>			m_vulkan;
	std::shared_ptr<const vk::UniqueCommandPool>	m_commandPool;
	vk::UniqueCommandBuffer							m_commandBuffer;

	static vk::UniqueCommandBuffer					createCommandBuffer(const Vulkan& vulkan,
																		vk::CommandBufferLevel level,
																		const std::shared_ptr<const vk::UniqueCommandPool>& pool);

};

}