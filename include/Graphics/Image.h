#pragma once

#include "Vulkan.h"
#include "Buffer.h"
#include "../Utils/BufferView.h"

#include <vector>
#include <tuple>

namespace Zuazo::Graphics {

class Image{
public:
	struct PlaneDescriptor {
		vk::Extent2D extent;
		vk::Format format;
		vk::ComponentMapping swizzle;
	};

	Image() = default;
	Image(	const Vulkan& vulkan,
			vk::ImageUsageFlags usage,
			vk::MemoryPropertyFlags memoryProperties,
			const Utils::BufferView<PlaneDescriptor>& imagePlanes );
	Image(const Image& other) = delete;
	Image(Image&& other) = default;
	~Image() = default;

	Image& 									operator=(const Image& other) = delete;
	Image& 									operator=(Image&& other) = default;

	const std::vector<vk::UniqueImage>&		getImages() const;
	const std::vector<std::pair<size_t, size_t>>& getPlanes() const;
	const vk::DeviceMemory&					getMemory() const;
	const std::vector<vk::UniqueImageView>&	getImageViews() const;

private:
	struct Memory {
		vk::UniqueDeviceMemory				memory;
		std::vector<std::pair<size_t, size_t>> planes;
	};

	std::vector<vk::UniqueImage> 			m_images;
	Memory									m_memory;
	std::vector<vk::UniqueImageView>		m_imageViews;

	static std::vector<vk::UniqueImage> 	createImages(	const Vulkan& vulkan,
															vk::ImageUsageFlags usage,
															const Utils::BufferView<PlaneDescriptor>& imagePlanes );
	static Memory							allocateMemory(	const Vulkan& vulkan,
															vk::MemoryPropertyFlags memoryProperties,
															const std::vector<vk::UniqueImage>& images );
	static std::vector<vk::UniqueImageView>	createImageViews(	const Vulkan& vulkan,
																const Utils::BufferView<PlaneDescriptor>& imagePlanes,
																const std::vector<vk::UniqueImage>& images );


};

}