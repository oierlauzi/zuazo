#include <zuazo/Graphics/TargetFramePool.h>

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
 * TargetFramePool::Impl
 */

struct TargetFramePool::Impl {
	class Allocator : public std::allocator<TargetFrame> {
	
	public:
		Allocator(const Impl& impl) 
			: m_pool(impl) 
		{
		}

		Allocator(const Allocator& other) = default;
		~Allocator() = default;

		Allocator& operator=(const Allocator& other) = default;

		void construct(TargetFrame* x) {
			const auto& pool = m_pool.get();

			new (x) TargetFrame(
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
	DepthStencilFormat								depthStencilFormat;
	std::shared_ptr<const TargetFrame::Cache> 		cache;

	mutable Utils::Pool<TargetFrame, Allocator>		framePool;

	Impl(	const Vulkan& vulkan, 
			const Frame::Descriptor& desc,
			DepthStencilFormat depthStencilFmt )
		: vulkan(vulkan)
		, frameDescriptor(Utils::makeShared<Frame::Descriptor>(desc))
		, depthStencilFormat(depthStencilFmt)
		, cache(TargetFrame::createCache(vulkan, desc, depthStencilFormat))
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

	DepthStencilFormat getDepthStencilFormat() const noexcept {
		return depthStencilFormat;
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


	std::shared_ptr<TargetFrame> acquireFrame() const {
		auto frame = framePool.acquire();
		frame->waitCompletion(Vulkan::NO_TIMEOUT);
		return frame;
	}


	const RenderPass& getRenderPass() const noexcept {
		return TargetFrame::getRenderPass(*cache);
	}

};



/*
 * TargetFramePool
 */

TargetFramePool::TargetFramePool(	const Vulkan& vulkan, 
									const Frame::Descriptor& desc,
									DepthStencilFormat depthStencilFmt )
	: m_impl({}, vulkan, desc, depthStencilFmt)
{
}


TargetFramePool::TargetFramePool(TargetFramePool&& other) noexcept = default;

TargetFramePool::~TargetFramePool() = default;

TargetFramePool& TargetFramePool::operator=(TargetFramePool&& other) noexcept = default;



const Vulkan& TargetFramePool::getVulkan() const noexcept {
	return m_impl->getVulkan();
}

const Frame::Descriptor& TargetFramePool::getFrameDescriptor() const noexcept {
	return m_impl->getFrameDescriptor();
}

DepthStencilFormat TargetFramePool::getDepthStencilFormat() const noexcept {
	return m_impl->getDepthStencilFormat();
}


void TargetFramePool::setMaxSpareCount(size_t spares) noexcept {
	m_impl->setMaxSpareCount(spares);
}

size_t TargetFramePool::getMaxSpareCount() const noexcept {
	return m_impl->getMaxSpareCount();
}

size_t TargetFramePool::getSpareCount() const noexcept {
	return m_impl->getSpareCount();
}


std::shared_ptr<TargetFrame> TargetFramePool::acquireFrame() const {
	return m_impl->acquireFrame();
}

const RenderPass& TargetFramePool::getRenderPass() const noexcept {
	return m_impl->getRenderPass();
}

}