#pragma once

#include "Vulkan.h"

namespace Zuazo::Graphics {

class CommandPool {
public:
	CommandPool() = default;
	CommandPool(const Vulkan& vulkan,
				uint32_t familyIndex,
				uint32_t bufferCount );
	CommandPool(const CommandPool& other) = delete;
	CommandPool(CommandPool&& other) = default;
	~CommandPool() = default;

	CommandPool& operator=(const CommandPool& other) = delete;
	CommandPool& operator=(CommandPool&& other) = default;

	vk::CommandPool	getCommandPool() const;
	const std::vector<vk::UniqueCommandBuffer>& getCommandBuffers() const;

private:
	vk::UniqueCommandPool 					m_commandPool;
	std::vector<vk::UniqueCommandBuffer>	m_commandBuffers;

	static vk::UniqueCommandPool 				createCommandPool(	const Vulkan& vulkan,
																	uint32_t familyIndex );
	static std::vector<vk::UniqueCommandBuffer>	createCommandBuffers(	const Vulkan& vulkan,
																		vk::CommandPool pool,
	 																	uint32_t bufferCount );

};

}