#include <zuazo/Graphics/Uploader.h>

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
 * Uploader::Impl
 */

struct Uploader::Impl {
	class Allocator : public std::allocator<StagedFrame> {
	
	public:
		Allocator(const Impl& impl) 
			: m_uploader(impl) 
		{
		}

		Allocator(const Allocator& other) = default;
		~Allocator() = default;

		Allocator& operator=(const Allocator& other) = default;

		void construct(StagedFrame* x) {
			const auto& uplo = m_uploader.get();

			new (x) StagedFrame(
				uplo.vulkan,
				uplo.frameDescriptor,
				uplo.colorTransferBuffer,
				uplo.planeDescriptors,
				uplo.commandPool
			);
		}

	private:
		std::reference_wrapper<const Impl>				m_uploader;

	};

	std::reference_wrapper<const Vulkan>			vulkan;
	std::shared_ptr<Frame::Descriptor>				frameDescriptor;
	InputColorTransfer								colorTransfer;
	std::vector<Frame::PlaneDescriptor>				planeDescriptors;

	std::shared_ptr<StagedBuffer>					colorTransferBuffer;
	std::shared_ptr<vk::UniqueCommandPool>			commandPool;

	mutable Utils::Pool<StagedFrame, Allocator>		framePool;


	Impl(	const Vulkan& vulkan, 
			const Frame::Descriptor& desc )
		: vulkan(vulkan)
		, frameDescriptor(Utils::makeShared<Frame::Descriptor>(desc))
		, colorTransfer(*frameDescriptor)
		, planeDescriptors(createPlaneDescriptors(vulkan, desc, colorTransfer))
		, colorTransferBuffer(Frame::createColorTransferBuffer(vulkan, colorTransfer))
		, commandPool(createCommandPool(vulkan))
		, framePool(1, Allocator(*this))
	{
	}

	~Impl() {
		colorTransferBuffer->waitCompletion(vulkan);
	}


	const Vulkan& getVulkan() const noexcept {
		return vulkan;
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
		frame->waitDependencies();
		return frame;
	}


	static const std::vector<vk::Format>& getSupportedFormats(const Vulkan& vulkan) {
		constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
			vk::FormatFeatureFlagBits::eSampledImage |
			vk::FormatFeatureFlagBits::eTransferDst ;

		return vulkan.listSupportedFormatsOptimal(DESIRED_FLAGS);
	}

private:
	static std::vector<Frame::PlaneDescriptor> createPlaneDescriptors(	const Vulkan& vulkan, 
																		const Frame::Descriptor& desc,
																		InputColorTransfer& colorTransfer )
	{
		std::vector<Frame::PlaneDescriptor> result = Frame::getPlaneDescriptors(desc);

		//Try to optimize it
		const auto supportedFormats = getSupportedFormats(vulkan);
		assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend()));

		colorTransfer.optimize(result, supportedFormats);
		for(auto& plane : result) {
			std::tie(plane.format, plane.swizzle) = optimizeFormat(std::make_tuple(plane.format, plane.swizzle));
		}

		return result;
	}

	static std::shared_ptr<vk::UniqueCommandPool> createCommandPool(const Vulkan& vulkan) {
		const vk::CommandPoolCreateInfo createInfo(
			{},													//Flags
			vulkan.getTransferQueueIndex()						//Queue index
		);

		return Utils::makeShared<vk::UniqueCommandPool>(vulkan.createCommandPool(createInfo));
	}

};



/*
 * Uploader
 */

Uploader::Uploader(	const Vulkan& vulkan, 
					const Frame::Descriptor& desc )
	: m_impl({}, vulkan, desc)
{
}

Uploader::Uploader(Uploader&& other) noexcept = default;

Uploader::~Uploader() = default;

Uploader& Uploader::operator=(Uploader&& other) noexcept = default;



const Vulkan& Uploader::getVulkan() const noexcept {
	return m_impl->getVulkan();
}


void Uploader::setMaxSpareCount(size_t spares) noexcept {
	m_impl->setMaxSpareCount(spares);
}

size_t Uploader::getMaxSpareCount() const noexcept {
	return m_impl->getMaxSpareCount();
}

size_t Uploader::getSpareCount() const noexcept {
	return m_impl->getSpareCount();
}


std::shared_ptr<StagedFrame> Uploader::acquireFrame() const {
	return m_impl->acquireFrame();
}


const std::vector<vk::Format>& Uploader::getSupportedFormats(const Vulkan& vulkan) {
	return Impl::getSupportedFormats(vulkan);
}

}