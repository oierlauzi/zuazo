#include <Graphics/Image.h>

#include <cassert>

namespace Zuazo::Graphics {

Image::Image(	const Vulkan& vulkan,
				vk::ImageUsageFlags usage,
				vk::MemoryPropertyFlags memoryProperties,
				const Utils::BufferView<vk::Format>&  formats, 
				const Utils::BufferView<vk::Extent2D>&  extents,
				const Utils::BufferView<vk::ComponentMapping>&  swizzles )
	: m_images(createImages(vulkan, usage, formats, extents))
	, m_imageViews(createImageViews(vulkan, formats, swizzles, m_images))
	, m_memory(allocateMemory(vulkan, memoryProperties, m_images))
{
}



std::vector<vk::UniqueImage> Image::createImages(	const Vulkan& vulkan,
													vk::ImageUsageFlags usage,
													const Utils::BufferView<vk::Format>& formats, 
													const Utils::BufferView<vk::Extent2D>& extents )
{
	assert(formats.size() == extents.size());
	std::vector<vk::UniqueImage> result;
	result.reserve(extents.size());

	for(size_t i = 0; i < extents.size(); i++){
		const vk::ImageCreateInfo createInfo(
			{},											//Flags
			vk::ImageType::e2D,							//Image type
			formats.data()[i],							//Pixel format
			vk::Extent3D(extents.data()[i], 1),			//Extent
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

std::vector<vk::UniqueImageView> Image::createImageViews(	const Vulkan& vulkan,
															const Utils::BufferView<vk::Format>& formats,
															const Utils::BufferView<vk::ComponentMapping>&  swizzles,
															const std::vector<vk::UniqueImage>& images )
{
	assert(formats.size() == images.size() && swizzles.size() == images.size());
	std::vector<vk::UniqueImageView> result;
	result.reserve(images.size());

	for(size_t i = 0; i < images.size(); i++){
		const vk::ImageViewCreateInfo createInfo(
			{},												//Flags
			*(images[i]),									//Image
			vk::ImageViewType::e2D,							//ImageView type
			formats.data()[i],								//Image format
			swizzles.data()[i],								//Swizzle
			vk::ImageSubresourceRange(						//Image subresources
				vk::ImageAspectFlagBits::eColor,				//Aspect mask
				0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
			)
		);

		result.emplace_back(vulkan.createImageView(createInfo));
	}

	return result;
}

Image::Memory Image::allocateMemory(const Vulkan& vulkan,
									vk::MemoryPropertyFlags memoryProperties,
									const std::vector<vk::UniqueImage>& images )
{
	Memory result;
	vk::MemoryRequirements requirements(0, 0, ~0);

	result.offsets.reserve(images.size());

	//Combine all fields and evaluate offsets
	for(size_t i = 0; i < images.size(); i++){
		const auto req = vulkan.getDevice().getImageMemoryRequirements(*(images[i]), vulkan.getDispatcher());

		if(requirements.size % req.alignment){
			//Padding is required for alignment
			requirements.size = (requirements.size / req.alignment + 1) * req.alignment;
		}
		result.offsets.emplace_back(requirements.size);

		requirements.size += req.size;
		requirements.memoryTypeBits &= req.memoryTypeBits;
		requirements.alignment = std::max(requirements.alignment, req.alignment);
	}

	result.memory = vulkan.allocateMemory(requirements, memoryProperties);

	//Bind image's memory
	for(size_t i = 0; i < images.size(); i++){
		vulkan.getDevice().bindImageMemory(*(images[i]), *result.memory, result.offsets[i], vulkan.getDispatcher());
	}

	return result;
}



}