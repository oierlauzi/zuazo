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
	return *(m_image.getPlanes().front().image);
}

vk::ImageView DepthStencil::getImageView() const noexcept {
	assert(m_image.getPlanes().size());
	return *(m_image.getPlanes().front().imageView);
}

vk::DeviceMemory DepthStencil::getDeviceMemory() const noexcept {
	return *(m_image.getMemory().memory);
}


Image DepthStencil::createImage(const Vulkan& vulkan, 
								vk::Extent2D ext,
								vk::Format fmt )
{
	assert(hasDepth(fmt) || hasStencil(fmt));

	const Image::PlaneDescriptor imagePlane = {
		ext,
		fmt,
		vk::ComponentMapping()
	};

	constexpr vk::ImageUsageFlags usage =
		vk::ImageUsageFlagBits::eDepthStencilAttachment;

	constexpr vk::ImageTiling tiling =
		vk::ImageTiling::eOptimal;

	constexpr vk::MemoryPropertyFlags memory =
		vk::MemoryPropertyFlagBits::eDeviceLocal;

	return Image(
		vulkan,
		imagePlane,
		usage,
		tiling,
		memory
	);
}

}