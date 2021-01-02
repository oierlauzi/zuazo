#include <zuazo/Graphics/Framebuffer.h>

#include <vector>
#include <algorithm>

namespace Zuazo::Graphics {

Framebuffer::Framebuffer(	const Vulkan& vulkan,
							Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
							const Image& image,
							const DepthStencil* depthStencil,
							RenderPass renderPass )
	: m_framebuffer(createFramebuffer(vulkan, planeDescriptors, image, depthStencil, renderPass))
{
}

vk::Framebuffer Framebuffer::get() const noexcept {
	return *m_framebuffer;
}



vk::UniqueFramebuffer Framebuffer::createFramebuffer(	const Vulkan& vulkan,
														Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
														const Image& image,
														const DepthStencil* depthStencil,
														RenderPass renderPass )
{
	assert(planeDescriptors.size() == image.getPlanes().size());
	const auto attachmentCount = image.getPlanes().size() + (depthStencil ? 1 : 0);

	//Enumerate all attachments
	std::vector<vk::ImageView> attachments;
	attachments.reserve(attachmentCount);
	std::transform(
		image.getPlanes().cbegin(), image.getPlanes().cend(),
		std::back_inserter(attachments),
		[] (const Image::Plane& plane) -> vk::ImageView {
			return *(plane.imageView);
		}
	);

	if(depthStencil) {
		attachments.emplace_back(depthStencil->getImageView());
	}

	assert(attachments.size() == attachmentCount);

	const vk::FramebufferCreateInfo createInfo(
		{},
		renderPass.get(),
		attachments.size(),
		attachments.data(),
		planeDescriptors.front().extent.width, planeDescriptors.front().extent.height,
		1
	);
	
	//Ensure that all planes have the same size
	for(size_t i = 1; i < planeDescriptors.size(); ++i) {
		assert(planeDescriptors[i].extent.width >= createInfo.width); 
		assert(planeDescriptors[i].extent.height >= createInfo.height); 
	}

	return vulkan.createFramebuffer(createInfo);
}

}