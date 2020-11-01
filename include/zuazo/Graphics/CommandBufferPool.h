#pragma once

#include "Vulkan.h"
#include "CommandBuffer.h"
#include "../Utils/Pimpl.h"

#include <memory>


namespace Zuazo::Graphics {

class CommandBufferPool {
public:
	CommandBufferPool(	const Vulkan& vulkan, 
						vk::CommandPoolCreateFlags flags,
						uint32_t queueFamilyIndex,
						vk::CommandBufferLevel level );
	CommandBufferPool(const CommandBufferPool& other) = delete;
	CommandBufferPool(CommandBufferPool&& other) noexcept;
	~CommandBufferPool();

	CommandBufferPool& 								operator=(const CommandBufferPool& other) = delete;
	CommandBufferPool& 								operator=(CommandBufferPool&& other) noexcept;

	const Vulkan&									getVulkan() const noexcept;

	void											setMaxSpareCount(size_t spares) noexcept;
	size_t											getMaxSpareCount() const noexcept;
	size_t											getSpareCount() const noexcept;

	std::shared_ptr<CommandBuffer>					acquireCommandBuffer() const;	

private:
	struct Impl;
	Utils::Pimpl<Impl>								m_impl;

};

}