#include <zuazo/Graphics/StagedFramePool.h>

#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/Buffer.h>
#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Utils/Pool.h>
#include <zuazo/Exception.h>

#include <utility>
#include <memory>
#include <iostream>

namespace Zuazo::Graphics {

/*
 * StagedFramePool::Impl
 */

struct StagedFramePool::Impl {
	class Allocator : public std::allocator<StagedFrame> {
	
	public:
		Allocator(const Impl& impl) 
			: m_pool(impl) 
		{
		}

		Allocator(const Allocator& other) = default;
		~Allocator() = default;

		Allocator& operator=(const Allocator& other) = default;

		void construct(StagedFrame* x) {
			const auto& pool = m_pool.get();

			new (x) StagedFrame(
				pool.vulkan,
				pool.frameDescriptor,
				pool.cache
			);
		}

	private:
		std::reference_wrapper<const Impl>				m_pool;

	};

	std::reference_wrapper<const Vulkan>			vulkan;
	std::shared_ptr<Frame::Descriptor>				frameDescriptor;
	std::shared_ptr<const StagedFrame::Cache> 		cache;


	mutable Utils::Pool<StagedFrame, Allocator>		framePool;

	Impl(	const Vulkan& vulkan, 
			const Frame::Descriptor& desc )
		: vulkan(vulkan)
		, frameDescriptor(Utils::makeShared<Frame::Descriptor>(desc))
		, cache(StagedFrame::createCache(vulkan, desc))
		, framePool(1, Allocator(*this))
	{
	}

	~Impl() = default;


	const Vulkan& getVulkan() const noexcept {
		return vulkan;
	}

	const Frame::Descriptor& getFrameDescriptor() const noexcept {
		assert(frameDescriptor);
		return *frameDescriptor;
	}


	void setMaxSpareCount(size_t spares) noexcept {
		framePool.setMaxSpareCount(spares);
	}

	size_t getMaxSpareCount() const noexcept {
		return framePool.getMaxSpareCount();
	}

	size_t getSpareCount() const noexcept {
		return framePool.getSpareCount();
	}


	std::shared_ptr<StagedFrame> acquireFrame() const {
		auto frame = framePool.acquire();
		frame->waitCompletion(Vulkan::NO_TIMEOUT);
		return frame;
	}

};



/*
 * StagedFramePool
 */

StagedFramePool::StagedFramePool(	const Vulkan& vulkan, 
									const Frame::Descriptor& desc )
	: m_impl({}, vulkan, desc)
{
}


StagedFramePool::StagedFramePool(StagedFramePool&& other) noexcept = default;

StagedFramePool::~StagedFramePool() = default;

StagedFramePool& StagedFramePool::operator=(StagedFramePool&& other) noexcept = default;



const Vulkan& StagedFramePool::getVulkan() const noexcept {
	return m_impl->getVulkan();
}

const Frame::Descriptor& StagedFramePool::getFrameDescriptor() const noexcept {
	return m_impl->getFrameDescriptor();
}


void StagedFramePool::setMaxSpareCount(size_t spares) noexcept {
	m_impl->setMaxSpareCount(spares);
}

size_t StagedFramePool::getMaxSpareCount() const noexcept {
	return m_impl->getMaxSpareCount();
}

size_t StagedFramePool::getSpareCount() const noexcept {
	return m_impl->getSpareCount();
}


std::shared_ptr<StagedFrame> StagedFramePool::acquireFrame() const {
	return m_impl->acquireFrame();
}

}