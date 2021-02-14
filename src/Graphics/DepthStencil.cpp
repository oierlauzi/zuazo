#include <zuazo/Graphics/DepthStencil.h>
#include <zuazo/Graphics/VulkanConversions.h>

namespace Zuazo::Graphics {

DepthStencil::DepthStencil(	const Vulkan& vulkan,
							vk::Extent2D ext,
							vk::Format fmt )
	: m_image(createImage(vulkan, ext, fmt))
{
}



vk::Image DepthStencil::getImage() const noexcept {
	assert(m_image.getPlanes().size());
	return m_image.getPlanes().front().getImage();
}

vk::ImageView DepthStencil::getImageView() const noexcept {
	assert(m_image.getPlanes().size());
	return m_image.getPlanes().front().getImageView();
}

vk::DeviceMemory DepthStencil::getDeviceMemory() const noexcept {
	return *(m_image.getMemory().memory);
}


Image DepthStencil::createImage(const Vulkan& vulkan, 
								vk::Extent2D ext,
								vk::Format fmt )
{
	assert(hasDepth(fmt) || hasStencil(fmt));

	const Image::Plane imagePlane(to3D(ext), fmt);

	constexpr vk::ImageUsageFlags usage =
		vk::ImageUsageFlagBits::eDepthStencilAttachment |
		vk::ImageUsageFlagBits::eTransientAttachment ; //No need to read it after the renderpass

	constexpr vk::ImageTiling tiling =
		vk::ImageTiling::eOptimal;

	constexpr vk::MemoryPropertyFlags memory =
		vk::MemoryPropertyFlagBits::eDeviceLocal |
		vk::MemoryPropertyFlagBits::eLazilyAllocated ;

	return Image(
		vulkan,
		imagePlane,
		usage,
		tiling,
		memory
	);
}

}
