#include <zuazo/Graphics/Uploader.h>

#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/Buffer.h>
#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Utils/Pool.h>
#include <zuazo/Exception.h>

#include <utility>
#include <memory>

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
			const Impl& uplo = m_uploader;

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
	std::shared_ptr<StagedBuffer>					colorTransferBuffer;
	std::vector<Frame::PlaneDescriptor>				planeDescriptors;

	std::shared_ptr<vk::UniqueCommandPool>			commandPool;

	mutable Utils::Pool<StagedFrame, Allocator>		framePool;


	Impl(	const Vulkan& vulkan, 
			const Frame::Descriptor& conf )
		: vulkan(vulkan)
		, frameDescriptor(Utils::makeShared<Frame::Descriptor>(conf))
		, colorTransfer(*frameDescriptor)
		, colorTransferBuffer(Frame::createColorTransferBuffer(vulkan, colorTransfer))
		, planeDescriptors(createPlaneDescriptors(vulkan, conf, colorTransfer))
		, commandPool(createCommandPool(vulkan))
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


	std::shared_ptr<StagedFrame> acquireFrame() const {
		auto frame = framePool.acquire();
		frame->waitDependecies();
		return frame;
	}

private:
	static std::vector<Frame::PlaneDescriptor> createPlaneDescriptors(	const Vulkan& vulkan, 
																		const Frame::Descriptor& desc,
																		InputColorTransfer& colorTransfer )
	{
		std::vector<Frame::PlaneDescriptor> result = Frame::getPlaneDescriptors(desc);

		//Try to optimize it
		const auto& supportedFormats = vulkan.getFormatSupport().sampler;
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
					const Frame::Descriptor& conf )
	: m_impl({}, vulkan, conf)
{
}

Uploader::Uploader(Uploader&& other) = default;

Uploader::~Uploader() = default;

Uploader& Uploader::operator=(Uploader&& other) = default;



const Vulkan& Uploader::getVulkan() const {
	return m_impl->getVulkan();
}


void Uploader::setMaxSpareCount(size_t spares) {
	m_impl->setMaxSpareCount(spares);
}

size_t Uploader::getMaxSpareCount() const {
	return m_impl->getMaxSpareCount();
}

size_t Uploader::getSpareCount() const {
	return m_impl->getSpareCount();
}


std::shared_ptr<StagedFrame> Uploader::acquireFrame() const {
	return m_impl->acquireFrame();
}

}