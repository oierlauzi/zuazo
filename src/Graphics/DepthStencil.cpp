#include <zuazo/Graphics/DepthStencil.h>
#include <zuazo/Graphics/VulkanConversions.h>

namespace Zuazo::Graphics {

DepthStencil::DepthStencil(	const Vulkan& vulkan,
							vk::Extent2D ext,
							vk::Format fmt )
	: m_image(createImage(vulkan, ext, fmt))
	, m_memory(createDeviceMemory(vulkan, *m_image))
	, m_imageView(createImageView(vulkan, fmt, *m_image))
{
}



vk::Image DepthStencil::getImage() const {
	return *m_image;
}

vk::DeviceMemory DepthStencil::getDeviceMemory() const {
	return *m_memory;
}

vk::ImageView DepthStencil::getImageView() const {
	return *m_imageView;
}


vk::UniqueImage DepthStencil::createImage(	const Vulkan& vulkan, 
									vk::Extent2D ext,
									vk::Format fmt )
{
	assert(hasDepth(fmt) || hasStencil(fmt));

	constexpr vk::ImageUsageFlags usage =
		vk::ImageUsageFlagBits::eDepthStencilAttachment;

	const vk::ImageCreateInfo createInfo(
		{},											//Flags
		vk::ImageType::e2D,							//Image type
		fmt,										//Pixel format
		vk::Extent3D(ext, 1), 						//Extent
		1,											//Mip levels
		1,											//Array layers
		vk::SampleCountFlagBits::e1,				//Sample count
		vk::ImageTiling::eOptimal,					//Tiling
		usage,										//Usage
		vk::SharingMode::eExclusive,				//Sharing mode
		0, nullptr,									//Queue family indices
		vk::ImageLayout::eUndefined					//Initial layout
	);

	return vulkan.createImage(createInfo);
}

vk::UniqueImageView DepthStencil::createImageView(	const Vulkan& vulkan, 
													vk::Format fmt, 
													vk::Image image )
{
	const vk::ImageViewCreateInfo createInfo(
		{},												//Flags
		image,											//Image
		vk::ImageViewType::e2D,							//ImageView type
		fmt,											//Image format
		vk::ComponentMapping(),							//Swizzle
		vk::ImageSubresourceRange(						//Image subresources
			vk::ImageAspectFlagBits::eColor,				//Aspect mask
			0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
		)
	);

	return vulkan.createImageView(createInfo);
}

vk::UniqueDeviceMemory DepthStencil::createDeviceMemory(const Vulkan& vulkan,
														vk::Image image )
{
	constexpr vk::MemoryPropertyFlags memoryProperties =
		vk::MemoryPropertyFlagBits::eDeviceLocal;

	const auto requirements = vulkan.getMemoryRequirements(image);
	auto result = vulkan.allocateMemory(requirements, memoryProperties);

	vulkan.bindMemory(image, *result);
	return result;
}

}