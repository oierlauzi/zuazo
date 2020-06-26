#include <Graphics/Uploader.h>

#include <Graphics/VulkanConversions.h>
#include <Exception.h>

namespace Zuazo::Graphics {

/*
 * Uploader
 */

Uploader::Uploader(	const Vulkan& vulkan, 
					const Frame::Descriptor& conf )
	: m_vulkan(vulkan)
	, m_frameDescriptor(std::make_shared<Frame::Descriptor>(conf))
	, m_colorTransfer(*m_frameDescriptor)
	, m_planeDescriptors(createPlaneDescriptors(vulkan, conf.getResolution(), conf.getColorSubsampling(), conf.getColorFormat(), m_colorTransfer))
	, m_commandPool(createCommandPool(m_vulkan))
	, m_colorTransferBuffer(Frame::createColorTransferBuffer(vulkan, m_colorTransfer))
	, m_pool(std::bind(&Uploader::allocateFrame, this))
{
}


std::shared_ptr<StagedFrame> Uploader::acquireFrame() const {
	auto frame = m_pool.acquire();
	frame->waitDependecies();
	return frame;
}

void Uploader::clear() {
	m_pool.clearUnused();
}



Utils::Pool<StagedFrame>::Ref Uploader::allocateFrame() const {
	return std::make_shared<StagedFrame>(
		m_vulkan,
		m_frameDescriptor,
		m_colorTransferBuffer,
		m_planeDescriptors,
		m_commandPool
	);
}

std::vector<Frame::PlaneDescriptor> Uploader::createPlaneDescriptors(	const Vulkan& vulkan, 
																		Resolution resolution,
																		ColorSubsampling subsampling,
																		ColorFormat format, 
																		ColorTransfer& colorTransfer)
{
	std::vector<Frame::PlaneDescriptor> result = Frame::getPlaneDescriptors(resolution, subsampling, format);

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

	return std::make_shared<vk::UniqueCommandPool>(vulkan.createCommandPool(createInfo));
}

}