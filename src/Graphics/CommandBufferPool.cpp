#include <zuazo/Graphics/CommandBufferPool.h>

#include <zuazo/Utils/Pool.h>
#include <zuazo/Exception.h>

#include <utility>
#include <memory>
#include <cassert>
#include <algorithm>

namespace Zuazo::Graphics {

/*
 * CommandBufferPool::Impl
 */

struct CommandBufferPool::Impl {
	class Allocator : public std::allocator<CommandBuffer> {
	
	public:
		Allocator(const Impl& impl) 
			: m_commandBufferPool(impl) 
		{
		}

		Allocator(const Allocator& other) = default;
		~Allocator() = default;

		Allocator& operator=(const Allocator& other) = default;

		void construct(CommandBuffer* x) {
			const auto& impl = m_commandBufferPool.get();

			new (x) CommandBuffer(
				impl.getVulkan(),
				impl.commandBufferLevel,
				impl.commandPool
			);
		}

	private:
		std::reference_wrapper<const Impl>				m_commandBufferPool;

	};

	std::reference_wrapper<const Vulkan>			vulkan;
	vk::CommandBufferLevel 							commandBufferLevel;

	std::shared_ptr<vk::UniqueCommandPool> 			commandPool;
	mutable Utils::Pool<CommandBuffer, Allocator>	pool;


	Impl(	const Vulkan& vulkan, 
			vk::CommandPoolCreateFlags flags,
			uint32_t queueFamilyIndex,
			vk::CommandBufferLevel level )
		: vulkan(vulkan)
		, commandBufferLevel(level)
		, commandPool(createCommandPool(vulkan, flags, queueFamilyIndex))
		, pool(1, Allocator(*this))
	{
	}

	~Impl() = default;

	const Vulkan& getVulkan() const noexcept {
		return vulkan;
	}


	void setMaxSpareCount(size_t spares) noexcept {
		pool.setMaxSpareCount(spares);
	}

	size_t getMaxSpareCount() const noexcept {
		return pool.getMaxSpareCount();
	}

	size_t getSpareCount() const noexcept {
		return pool.getSpareCount();
	}


	std::shared_ptr<CommandBuffer> acquireCommandBuffer() const {
		auto result = pool.acquire();
		assert(result);
		result->clearDependencies();

		return result;
	}

private:
	static std::shared_ptr<vk::UniqueCommandPool> createCommandPool(const Vulkan& vulkan,
																	vk::CommandPoolCreateFlags flags,
																	uint32_t queueFamilyIndex )
	{
		const vk::CommandPoolCreateInfo createInfo(
			flags,
			queueFamilyIndex
		);

		return Utils::makeShared<vk::UniqueCommandPool>(vulkan.createCommandPool(createInfo));
	}
};



/*
 * Drawtable
 */

CommandBufferPool::CommandBufferPool(	const Vulkan& vulkan, 
										vk::CommandPoolCreateFlags flags,
										uint32_t queueFamilyIndex,
										vk::CommandBufferLevel level  )
	: m_impl({}, vulkan, flags, queueFamilyIndex, level)
{
}

CommandBufferPool::CommandBufferPool(CommandBufferPool&& other) noexcept = default;

CommandBufferPool::~CommandBufferPool() = default;

CommandBufferPool& CommandBufferPool::operator=(CommandBufferPool&& other) noexcept = default;



const Vulkan& CommandBufferPool::getVulkan() const noexcept {
	return m_impl->getVulkan();
}


void CommandBufferPool::setMaxSpareCount(size_t spares) noexcept {
	m_impl->setMaxSpareCount(spares);
}

size_t CommandBufferPool::getMaxSpareCount() const noexcept {
	return m_impl->getMaxSpareCount();
}

size_t CommandBufferPool::getSpareCount() const noexcept {
	return m_impl->getSpareCount();
}


std::shared_ptr<CommandBuffer> CommandBufferPool::acquireCommandBuffer() const {
	return m_impl->acquireCommandBuffer();
}

}