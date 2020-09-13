#include <zuazo/Graphics/Uploader.h>

#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/Buffer.h>
#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Utils/Pool.h>
#include <zuazo/Exception.h>

#include <utility>

namespace Zuazo::Graphics {

/*
 * Uploader::Impl
 */

struct Uploader::Impl {
	struct Allocator {
		std::reference_wrapper<const Uploader::Impl> uploader;

		std::shared_ptr<StagedFrame> operator()() const {
			return Utils::makeShared<StagedFrame>(
				uploader.get().vulkan,
				uploader.get().frameDescriptor,
				uploader.get().colorTransferBuffer,
				uploader.get().planeDescriptors,
				uploader.get().commandPool
			);
		}
	};

	std::reference_wrapper<const Vulkan>			vulkan;
	std::shared_ptr<Frame::Descriptor>				frameDescriptor;
	InputColorTransfer								colorTransfer;
	std::vector<Frame::PlaneDescriptor>				planeDescriptors;

	std::shared_ptr<vk::UniqueCommandPool>			commandPool;
	std::shared_ptr<StagedBuffer>					colorTransferBuffer;

	mutable Utils::Pool<StagedFrame, Allocator>		framePool;


	Impl(	const Vulkan& vulkan, 
			const Frame::Descriptor& conf )
		: vulkan(vulkan)
		, frameDescriptor(std::make_shared<Frame::Descriptor>(conf))
		, colorTransfer(*frameDescriptor)
		, planeDescriptors(createPlaneDescriptors(vulkan, conf, colorTransfer))
		, commandPool(createCommandPool(vulkan))
		, colorTransferBuffer(Frame::createColorTransferBuffer(vulkan, colorTransfer))
		, framePool(Allocator{*this})
	{
	}

	~Impl() {
		colorTransferBuffer->waitCompletion(vulkan);
	}


	const Vulkan& getVulkan() const {
		return vulkan;
	}

	std::shared_ptr<StagedFrame> acquireFrame() const {
		auto frame = framePool.acquire();
		frame->waitDependecies();
		return frame;
	}

	void clear() {
		framePool.clear();
	}

	void shrink(size_t maxSpares) {
		framePool.shrink(maxSpares);
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

std::shared_ptr<StagedFrame> Uploader::acquireFrame() const {
	return m_impl->acquireFrame();
}

void Uploader::clear() {
	m_impl->clear();
}

void Uploader::shrink(size_t maxSpares) {
	m_impl->shrink(maxSpares);
}

}