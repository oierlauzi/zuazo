#include <zuazo/Graphics/Drawtable.h>

#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/Buffer.h>
#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Utils/Pool.h>
#include <zuazo/Exception.h>

#include <utility>
#include <memory>
#include <cassert>
#include <algorithm>

namespace Zuazo::Graphics {

/*
 * Drawtable::Impl
 */

struct Drawtable::Impl {
	class Allocator : public std::allocator<TargetFrame> {
	
	public:
		Allocator(const Impl& impl) 
			: m_drawtable(impl) 
		{
		}

		Allocator(const Allocator& other) = default;
		~Allocator() = default;

		Allocator& operator=(const Allocator& other) = default;

		void construct(TargetFrame* x) {
			const Impl& drw = m_drawtable;

			new (x) TargetFrame(
				drw.vulkan,
				drw.frameDescriptor,
				drw.colorTransferBuffer,
				drw.planeDescriptors,
				drw.renderPass,
				drw.depthStencil
			);
		}

	private:
		std::reference_wrapper<const Impl>				m_drawtable;

	};

	std::reference_wrapper<const Vulkan>			vulkan;
	std::shared_ptr<Frame::Descriptor>				frameDescriptor;
	InputColorTransfer								colorTransfer;
	std::shared_ptr<StagedBuffer>					colorTransferBuffer;
	std::vector<Frame::PlaneDescriptor>				planeDescriptors;

	std::shared_ptr<const vk::UniqueRenderPass> 	renderPass;
	std::shared_ptr<DepthStencil>					depthStencil;

	mutable Utils::Pool<TargetFrame, Allocator>		framePool;


	Impl(	const Vulkan& vulkan, 
			const Frame::Descriptor& conf,
			std::shared_ptr<const vk::UniqueRenderPass> renderPass,
			vk::Format depthStencilFmt )
		: vulkan(vulkan)
		, frameDescriptor(Utils::makeShared<Frame::Descriptor>(conf))
		, colorTransfer(*frameDescriptor)
		, colorTransferBuffer(Frame::createColorTransferBuffer(vulkan, colorTransfer))
		, planeDescriptors(createPlaneDescriptors(vulkan, conf, colorTransfer))
		, renderPass(std::move(renderPass))
		, depthStencil(createDepthStencil(vulkan, depthStencilFmt, planeDescriptors))
		, framePool(1, Allocator(*this))
	{
	}

	~Impl() {
		colorTransferBuffer->waitCompletion(vulkan);
	}


	const Vulkan& getVulkan() const {
		return vulkan;
	}


	void setMaxSpareCount(size_t spares) {
		framePool.setMaxSpareCount(spares);
	}

	size_t getMaxSpareCount() const {
		return framePool.getMaxSpareCount();
	}

	size_t getSpareCount() const {
		return framePool.getSpareCount();
	}


	std::shared_ptr<TargetFrame> acquireFrame() const {
		return framePool.acquire();
	}

	OutputColorTransfer getOutputColorTransfer() const {
		return OutputColorTransfer(colorTransfer);
	}

private:
	static std::vector<Frame::PlaneDescriptor> createPlaneDescriptors(	const Vulkan& vulkan, 
																		const Frame::Descriptor& desc,
																		InputColorTransfer& colorTransfer )
	{
		std::vector<Frame::PlaneDescriptor> result = Frame::getPlaneDescriptors(desc);

		//Try to optimize it
		const auto& supportedFormats = vulkan.getFormatSupport().framebuffer;
		assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend())); //For binary search

		for(auto& plane : result) {
			std::tie(plane.format, plane.swizzle) = optimizeFormat(std::make_tuple(plane.format, plane.swizzle));

			//Ensure that the format is supported
			assert(plane.swizzle == vk::ComponentMapping());
			assert(std::binary_search(supportedFormats.cbegin(), supportedFormats.cend(), plane.format));
		}

		colorTransfer.optimize(result, supportedFormats);

		return result;
	}

	static std::shared_ptr<DepthStencil> createDepthStencil(const Vulkan& vulkan,
															vk::Format format,
															const std::vector<Frame::PlaneDescriptor>& desc)
	{
		std::shared_ptr<DepthStencil> result;

		if(format != vk::Format::eUndefined) {
			//We need to create a depth buffer
			assert(desc.size() > 0);

			result = Utils::makeShared<DepthStencil>(
				vulkan,
				desc.front().extent,
				format
			);
		}

		return result;
	}
};



/*
 * Drawtable
 */

Drawtable::Drawtable(	const Vulkan& vulkan, 
						const Frame::Descriptor& conf,
						std::shared_ptr<const vk::UniqueRenderPass> renderPass,
						vk::Format depthStencilFmt )
	: m_impl({}, vulkan, conf, std::move(renderPass), depthStencilFmt)
{
}

Drawtable::Drawtable(Drawtable&& other) = default;

Drawtable::~Drawtable() = default;

Drawtable& Drawtable::operator=(Drawtable&& other) = default;



const Vulkan& Drawtable::getVulkan() const {
	return m_impl->getVulkan();
}


void Drawtable::setMaxSpareCount(size_t spares) {
	m_impl->setMaxSpareCount(spares);
}

size_t Drawtable::getMaxSpareCount() const {
	return m_impl->getMaxSpareCount();
}

size_t Drawtable::getSpareCount() const {
	return m_impl->getSpareCount();
}


std::shared_ptr<TargetFrame> Drawtable::acquireFrame() const {
	return m_impl->acquireFrame();
}

OutputColorTransfer Drawtable::getOutputColorTransfer() const {
	return m_impl->getOutputColorTransfer();
}

}