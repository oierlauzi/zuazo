#include <zuazo/Graphics/Image.h>

#include <zuazo/Graphics/VulkanConversions.h>

namespace Zuazo::Graphics {

Image::Image(	const Vulkan& vulkan,
				Utils::BufferView<const PlaneDescriptor> planesDescriptors,
				vk::ImageUsageFlags usage )
	: m_imagePlanes(createPlanes(vulkan, planesDescriptors, usage))
	, m_memory(createMemory(vulkan, m_imagePlanes))
{
	for(size_t i = 0; i < m_imagePlanes.size(); ++i) {
		const auto image = *m_imagePlanes[i].image;

		//Bind the memory memory to the image
		vulkan.bindMemory(image, *m_memory.memory, m_memory.areas[i].offset());

		//Image views must be created once the memory is bound
		m_imagePlanes[i].imageView = createImageView(vulkan, planesDescriptors[i], image);
	}
}


Utils::BufferView<const Image::Plane> Image::getPlanes() const {
	return m_imagePlanes;
}

const Vulkan::AggregatedAllocation& Image::getMemory() const {
	return m_memory;
}


vk::UniqueImage Image::createImage(	const Vulkan& vulkan,
									const PlaneDescriptor& desc, 
									vk::ImageUsageFlags usage )
{
	const vk::ImageCreateInfo createInfo(
		{},											//Flags
		vk::ImageType::e2D,							//Image type
		desc.format,								//Pixel format
		vk::Extent3D(desc.extent, 1), 				//Extent
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

vk::UniqueImageView Image::createImageView(	const Vulkan& vulkan,
											const PlaneDescriptor& desc,
											vk::Image image )
{
	//Decide the aspect mask of the image view
	vk::ImageAspectFlags aspectMask;

	if(hasDepth(desc.format)) {
		aspectMask |= vk::ImageAspectFlagBits::eDepth;
	}

	if(hasStencil(desc.format)) {
		aspectMask |= vk::ImageAspectFlagBits::eStencil;
	}

	if(aspectMask == vk::ImageAspectFlags()) {
		//None was selected, default to color
		aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	const vk::ImageSubresourceRange subresourceRange(
		vk::ImageAspectFlagBits::eColor,				//Aspect mask
		0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
	);

	const vk::ImageViewCreateInfo createInfo(
		{},												//Flags
		image,											//Image
		vk::ImageViewType::e2D,							//ImageView type
		desc.format,									//Image format
		desc.swizzle,									//Swizzle
		subresourceRange								//Image subresources
	);

	return vulkan.createImageView(createInfo);
}

std::vector<Image::Plane> Image::createPlanes(	const Vulkan& vulkan,
												Utils::BufferView<const PlaneDescriptor> desc, 
												vk::ImageUsageFlags usage )
{
	std::vector<Plane> result;
	result.reserve(desc.size());

	for(const auto& d : desc) {
		result.emplace_back(Plane{ createImage(vulkan, d, usage), vk::UniqueImageView() });
	}

	assert(result.size() == desc.size());
	return result;
}

Vulkan::AggregatedAllocation Image::createMemory(	const Vulkan& vulkan,
													const std::vector<Plane>& planes )
{
	constexpr vk::MemoryPropertyFlags memoryProperties =
		vk::MemoryPropertyFlagBits::eDeviceLocal;

	std::vector<vk::MemoryRequirements> requirements;
	requirements.reserve(planes.size());

	for(const auto& plane : planes) {
		requirements.push_back(vulkan.getMemoryRequirements(*plane.image));
	}
	assert(requirements.size() == planes.size());

	auto result = vulkan.allocateMemory(requirements, memoryProperties);
	assert(result.areas.size() == planes.size());

	return result;
}

}