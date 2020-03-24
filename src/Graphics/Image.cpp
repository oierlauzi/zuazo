#include <Graphics/Image.h>

#include <cassert>
#include <iostream>

namespace Zuazo::Graphics {

Image::Image(	const Vulkan& vulkan,
				vk::ImageUsageFlags usage,
				vk::MemoryPropertyFlags memoryProperties,
				const Utils::BufferView<PlaneDescriptor>& imagePlanes )
	: m_images(createImages(vulkan, usage, imagePlanes))
	, m_memory(allocateMemory(vulkan, memoryProperties, m_images))
	, m_imageViews(createImageViews(vulkan, imagePlanes, m_images))
{
}

const std::vector<vk::UniqueImage>& Image::getImages() const{
	return m_images;
}

const std::vector<std::pair<size_t, size_t>>& Image::getPlanes() const {
	return m_memory.planes;
}

const vk::DeviceMemory& Image::getMemory() const {
	return *(m_memory.memory);
}


const std::vector<vk::UniqueImageView>&	Image::getImageViews() const{
	return m_imageViews;
}



std::vector<vk::UniqueImage> Image::createImages(	const Vulkan& vulkan,
													vk::ImageUsageFlags usage,
													const Utils::BufferView<PlaneDescriptor>& imagePlanes )
{
	std::vector<vk::UniqueImage> result;
	result.reserve(imagePlanes.size());

	for(size_t i = 0; i < imagePlanes.size(); i++){
		const vk::ImageCreateInfo createInfo(
			{},											//Flags
			vk::ImageType::e2D,							//Image type
			imagePlanes[i].format,						//Pixel format
			vk::Extent3D(imagePlanes[i].extent, 1), 	//Extent
			1,											//Mip levels
			1,											//Array layers
			vk::SampleCountFlagBits::e1,				//Sample count
			vk::ImageTiling::eOptimal,					//Tiling
			usage,										//Usage
			vk::SharingMode::eExclusive,				//Sharing mode
			0, nullptr,									//Queue family indices
			vk::ImageLayout::eUndefined					//Initial layout
		);

		result.emplace_back(vulkan.createImage(createInfo));
	}
	
	return result;
}

Image::Memory Image::allocateMemory(const Vulkan& vulkan,
									vk::MemoryPropertyFlags memoryProperties,
									const std::vector<vk::UniqueImage>& images )
{
	Memory result;
	vk::MemoryRequirements requirements(0, 1, ~(0U));

	result.planes.reserve(images.size());

	//Combine all fields and evaluate offsets
	for(size_t i = 0; i < images.size(); i++){
		const auto req = vulkan.getDevice().getImageMemoryRequirements(*(images[i]), vulkan.getDispatcher());

		if(requirements.size % req.alignment){
			//Padding is required for alignment
			requirements.size = (requirements.size / req.alignment + 1) * req.alignment;
		}
		result.planes.emplace_back(requirements.size, req.size);

		requirements.size += req.size;
		requirements.memoryTypeBits &= req.memoryTypeBits;
		requirements.alignment = std::max(requirements.alignment, req.alignment);
	}

	result.memory = vulkan.allocateMemory(requirements, memoryProperties);

	//Bind image's memory
	for(size_t i = 0; i < images.size(); i++){
		vulkan.getDevice().bindImageMemory(*(images[i]), *result.memory, result.planes[i].first, vulkan.getDispatcher());
	}

	return result;
}

std::vector<vk::UniqueImageView> Image::createImageViews(	const Vulkan& vulkan,
															const Utils::BufferView<PlaneDescriptor>& imagePlanes ,
															const std::vector<vk::UniqueImage>& images )
{
	assert(imagePlanes.size() == images.size());
	std::vector<vk::UniqueImageView> result;
	result.reserve(images.size());

	for(size_t i = 0; i < images.size(); i++){
		const vk::ImageViewCreateInfo createInfo(
			{},												//Flags
			*(images[i]),									//Image
			vk::ImageViewType::e2D,							//ImageView type
			imagePlanes[i].format,							//Image format
			imagePlanes[i].swizzle,							//Swizzle
			vk::ImageSubresourceRange(						//Image subresources
				vk::ImageAspectFlagBits::eColor,				//Aspect mask
				0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
			)
		);

		result.emplace_back(vulkan.createImageView(createInfo));
	}

	return result;
}

}