#pragma once

#include "Vulkan.h"
#include "Buffer.h"
#include "../Resolution.h"
#include "../ColorSubsampling.h"
#include "../ColorFormat.h"
#include "../Utils/BufferView.h"

#include <vector>
#include <utility>

namespace Zuazo::Graphics {

class Image{
public:
	struct PlaneDescriptor {
		vk::Extent2D extent;
		vk::Format format;
		vk::ComponentMapping swizzle;
	};

	using PlaneOffset = std::pair<size_t, size_t>;

	using PixelData = std::vector<Utils::BufferView<std::byte>>;

	Image() = default;
	Image(	const Vulkan& vulkan,
			vk::ImageUsageFlags usage,
			vk::MemoryPropertyFlags memoryProperties,
			Utils::BufferView<const PlaneDescriptor> imagePlanes );
	Image(const Image& other) = delete;
	Image(Image&& other) = default;
	~Image() = default;

	Image& 									operator=(const Image& other) = delete;
	Image& 									operator=(Image&& other) = default;

	const std::vector<vk::UniqueImage>&		getImages() const;
	const std::vector<PlaneOffset>&			getPlanes() const;
	const vk::DeviceMemory&					getMemory() const;
	const std::vector<vk::UniqueImageView>&	getImageViews() const;

	static std::vector<PlaneDescriptor>		getPlaneDescriptors(Resolution resolution,
																ColorSubsampling subsampling,
																ColorFormat format );
	static PixelData						slice(	std::byte* data, 
													Utils::BufferView<const PlaneOffset> offsets );
private:
	std::vector<vk::UniqueImage> 			m_images;
	Vulkan::AggregatedAllocation			m_memory;
	std::vector<vk::UniqueImageView>		m_imageViews;

	static std::vector<vk::UniqueImage> 	createImages(	const Vulkan& vulkan,
															vk::ImageUsageFlags usage,
															Utils::BufferView<const PlaneDescriptor> imagePlanes );
	static Vulkan::AggregatedAllocation		allocateMemory(	const Vulkan& vulkan,
															vk::MemoryPropertyFlags memoryProperties,
															const std::vector<vk::UniqueImage>& images );
	static std::vector<vk::UniqueImageView>	createImageViews(	const Vulkan& vulkan,
																Utils::BufferView<const PlaneDescriptor> imagePlanes,
																const std::vector<vk::UniqueImage>& images );


};

}