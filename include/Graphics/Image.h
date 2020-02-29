#pragma once

#include "Vulkan.h"
#include "Buffer.h"

#include <vector>
#include <span>
#include <tuple>

namespace Zuazo::Graphics {

class Image{
public:
	Image() = default;
	Image(	const Vulkan& vulkan,
			vk::ImageUsageFlags usage,
			vk::MemoryPropertyFlags memoryProperties,
			const std::span<const std::tuple<vk::Extent2D, vk::Format, vk::ComponentMapping>>& imagePlanes );
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
															const std::span<const std::tuple<vk::Extent2D, vk::Format, vk::ComponentMapping>>& imagePlanes );
	static Memory							allocateMemory(	const Vulkan& vulkan,
															vk::MemoryPropertyFlags memoryProperties,
															const std::vector<vk::UniqueImage>& images );
	static std::vector<vk::UniqueImageView>	createImageViews(	const Vulkan& vulkan,
																const std::span<const std::tuple<vk::Extent2D, vk::Format, vk::ComponentMapping>>& imagePlanes,
																const std::vector<vk::UniqueImage>& images );


};

}