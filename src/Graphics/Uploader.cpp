#include <zuazo/Graphics/Uploader.h>

#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Exception.h>

namespace Zuazo::Graphics {

/*
 * Uploader
 */

Uploader::Uploader(	const Vulkan& vulkan, 
					const Frame::Descriptor& conf )
	: m_vulkan(vulkan)
	, m_frameDescriptor(std::make_shared<Frame::Descriptor>(conf))
	, m_colorTransfer(*m_frameDescriptor)
	, m_planeDescriptors(createPlaneDescriptors(vulkan, conf, m_colorTransfer))
	, m_commandPool(createCommandPool(m_vulkan))
	, m_colorTransferBuffer(Frame::createColorTransferBuffer(vulkan, m_colorTransfer))
	, m_pool(Allocator(*this))
{
}


std::shared_ptr<StagedFrame> Uploader::acquireFrame() const {
	auto frame = m_pool.acquire();
	frame->waitDependecies();
	return frame;
}

void Uploader::clear() {
	m_pool.clear();
}

void Uploader::shrink(size_t maxSpares) {
	m_pool.shrink(maxSpares);
}


std::vector<Frame::PlaneDescriptor> Uploader::createPlaneDescriptors(	const Vulkan& vulkan, 
																		const Frame::Descriptor& desc, 
																		ColorTransfer& colorTransfer)
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

std::shared_ptr<vk::UniqueCommandPool> Uploader::createCommandPool(const Vulkan& vulkan){
	const vk::CommandPoolCreateInfo createInfo(
		{},													//Flags
		vulkan.getTransferQueueIndex()						//Queue index
	);

	return Utils::makeShared<vk::UniqueCommandPool>(vulkan.createCommandPool(createInfo));
}

Uploader::Allocator::Allocator(const Uploader& uploader)
	: m_uploader(uploader)
{
}

std::shared_ptr<StagedFrame> Uploader::Allocator::operator()() const {
	return Utils::makeShared<StagedFrame>(
		m_uploader.get().m_vulkan,
		m_uploader.get().m_frameDescriptor,
		m_uploader.get().m_colorTransferBuffer,
		m_uploader.get().m_planeDescriptors,
		m_uploader.get().m_commandPool
	);
}

}