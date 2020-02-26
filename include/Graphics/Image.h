#pragma once

#include "Vulkan.h"
#include "Buffer.h"
#include "../Utils/BufferView.h"

#include <vector>

namespace Zuazo::Graphics {

class Image{
public:
	Image() = default;
	Image(	const Vulkan& vulkan,
			vk::ImageUsageFlags usage,
			vk::MemoryPropertyFlags memoryProperties,
			const Utils::BufferView<vk::Format>&  formats, 
			const Utils::BufferView<vk::Extent2D>&  extents,
			const Utils::BufferView<vk::ComponentMapping>&  swizzles );
	Image(const Image& other) = delete;
	Image(Image&& other) = default;
	~Image() = default;

	Image& 									operator=(const Image& other) = delete;
	Image& 									operator=(Image&& other) = default;

	const std::vector<vk::UniqueImageView>&	getImageViews() const;

private:
	struct Memory {
		vk::UniqueDeviceMemory				memory;
		std::vector<size_t>					offsets;
	};

	std::vector<vk::UniqueImage> 			m_images;
	Memory									m_memory;
	std::vector<vk::UniqueImageView>		m_imageViews;

	static std::vector<vk::UniqueImage> 	createImages(	const Vulkan& vulkan,
															vk::ImageUsageFlags usage,
															const Utils::BufferView<vk::Format>& formats, 
															const Utils::BufferView<vk::Extent2D>&  extents );
	static Memory							allocateMemory(	const Vulkan& vulkan,
															vk::MemoryPropertyFlags memoryProperties,
															const std::vector<vk::UniqueImage>& images );
	static std::vector<vk::UniqueImageView>	createImageViews(	const Vulkan& vulkan,
																const Utils::BufferView<vk::Format>& formats,
																const Utils::BufferView<vk::ComponentMapping>&  swizzles,
																const std::vector<vk::UniqueImage>& images );


};

}