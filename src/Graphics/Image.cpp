#include <Graphics/Image.h>

#include <Graphics/VulkanConversions.h>

#include <cassert>
#include <iostream>

namespace Zuazo::Graphics {

Image::Image(	const Vulkan& vulkan,
				vk::ImageUsageFlags usage,
				vk::MemoryPropertyFlags memoryProperties,
				Utils::BufferView<const PlaneDescriptor> imagePlanes )
	: m_images(createImages(vulkan, usage, imagePlanes))
	, m_memory(allocateMemory(vulkan, memoryProperties, m_images))
	, m_imageViews(createImageViews(vulkan, imagePlanes, m_images))
{
}

const std::vector<vk::UniqueImage>& Image::getImages() const{
	return m_images;
}

const std::vector<Image::PlaneOffset>& Image::getPlanes() const {
	return m_memory.offsets;
}

const vk::DeviceMemory& Image::getMemory() const {
	return *(m_memory.memory);
}


const std::vector<vk::UniqueImageView>&	Image::getImageViews() const{
	return m_imageViews;
}


std::vector<Image::PlaneDescriptor> Image::getPlaneDescriptors(	Resolution resolution,
																ColorSubsampling subsampling,
																ColorFormat format )
{
	const auto planeCount = getPlaneCount(format);
	std::vector<Image::PlaneDescriptor> result;
	result.reserve(planeCount);

	const auto formats = toVulkan(format);
	const auto extent = toVulkan(resolution);
	const auto subsampledExtent = toVulkan(getSubsampledResolution(subsampling, resolution));

	assert(planeCount <= formats.size());

	for(size_t i = 0; i < planeCount; i++) {
		result.push_back(
			PlaneDescriptor {
				(i == 0) ? extent : subsampledExtent,
				std::get<vk::Format>(formats[i]),
				std::get<vk::ComponentMapping>(formats[i])
			}
		);
	}

	return result;
}

Image::PixelData Image::slice(	std::byte* data, 
								Utils::BufferView<const PlaneOffset> offsets )
{
	PixelData result;
	result.reserve(offsets.size());

	for(size_t i = 0; i < offsets.size(); i++) {
		result.emplace_back(data + offsets[i].first, offsets[i].second);
	}

	return result;
}



std::vector<vk::UniqueImage> Image::createImages(	const Vulkan& vulkan,
													vk::ImageUsageFlags usage,
													Utils::BufferView<const PlaneDescriptor> imagePlanes )
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

Vulkan::AggregatedAllocation Image::allocateMemory(	const Vulkan& vulkan,
													vk::MemoryPropertyFlags memoryProperties,
													const std::vector<vk::UniqueImage>& images )
{
	std::vector<vk::MemoryRequirements> requirements;
	requirements.reserve(images.size());

	for(size_t i = 0; i < images.size(); i++) {
		requirements.push_back(vulkan.getMemoryRequirements(*images[i]));
	}

	auto result = vulkan.allocateMemory(requirements, memoryProperties);
	assert(result.offsets.size() == images.size());

	//Bind image's memory
	for(size_t i = 0; i < images.size(); i++){
		vulkan.bindMemory(*images[i], *result.memory, result.offsets[i].first);
	}

	return result;
}

std::vector<vk::UniqueImageView> Image::createImageViews(	const Vulkan& vulkan,
															Utils::BufferView<const PlaneDescriptor> imagePlanes,
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