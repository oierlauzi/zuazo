#include <zuazo/Graphics/Framebuffer.h>

#include <vector>
#include <algorithm>

namespace Zuazo::Graphics {

Framebuffer::Framebuffer(	const Vulkan& vulkan,
							const Image& image,
							const Image* intermediaryImage,
							const DepthStencil* depthStencil,
							RenderPass renderPass )
	: m_framebuffer(createFramebuffer(vulkan, image, intermediaryImage, depthStencil, renderPass))
{
}

vk::Framebuffer Framebuffer::get() const noexcept {
	return *m_framebuffer;
}



vk::UniqueFramebuffer Framebuffer::createFramebuffer(	const Vulkan& vulkan,
														const Image& image,
														const Image* intermediaryImage,
														const DepthStencil* depthStencil,
														RenderPass renderPass )
{
	const auto attachmentCount = image.getPlanes().size() + (intermediaryImage ? 1 : 0) + (depthStencil ? 1 : 0);
	const auto extent = image.getPlanes().front().getExtent();

	//Enumerate all attachments
	std::vector<vk::ImageView> attachments;
	attachments.reserve(attachmentCount);

	//First comes the depth image, if present
	if(depthStencil) {
		attachments.emplace_back(depthStencil->getImageView());
	}

	//Then comes the intermediary image, if present
	if(intermediaryImage) {
		assert(intermediaryImage->getPlanes().size() == 1);
		attachments.emplace_back(intermediaryImage->getPlanes().front().getImageView());
	}

	//Finally comes the result image
	std::transform(
		image.getPlanes().cbegin(), image.getPlanes().cend(),
		std::back_inserter(attachments),
		[] (const Image::Plane& plane) -> vk::ImageView {
			return plane.getImageView();
		}
	);

	assert(attachments.size() == attachmentCount);

	const vk::FramebufferCreateInfo createInfo(
		{},
		renderPass.get(),
		attachments.size(),
		attachments.data(),
		extent.width, extent.height,
		1
	);

	return vulkan.createFramebuffer(createInfo);
}

}