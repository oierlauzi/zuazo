#pragma once

#include "Vulkan.h"
#include "../Utils/BufferView.h"

#include <vector>

namespace Zuazo::Graphics {

class Image {
public:
	struct PlaneDescriptor {
		vk::Extent2D 						extent;
		vk::Format 							format;
		vk::ComponentMapping 				swizzle;
	};

	struct Plane {
		vk::UniqueImage						image;
		vk::UniqueImageView					imageView;
	};

	Image() = default;
	Image(	const Vulkan& vulkan,
			Utils::BufferView<const PlaneDescriptor> planeDescriptors,
			vk::ImageUsageFlags usage,
			vk::ImageTiling tiling,
			vk::MemoryPropertyFlags memory );
	Image(	const Vulkan& vulkan,
			Utils::BufferView<const PlaneDescriptor> planeDescriptors,
			Utils::BufferView<const vk::Image> planes,
			vk::ImageUsageFlags usage );
	Image(const Image& other) = delete;
	Image(Image&& other) = default;
	~Image() = default;

	Image&								operator=(const Image& other) = delete;
	Image&								operator=(Image&& other) = default;

	Utils::BufferView<const Plane>		getPlanes() const;
	const Vulkan::AggregatedAllocation&	getMemory() const;

private:
	std::vector<Plane>					m_imagePlanes;
	Vulkan::AggregatedAllocation		m_memory;

	void								bindMemory(const Vulkan& vulkan);
	void								createImageViews(	const Vulkan& vulkan,
															Utils::BufferView<const PlaneDescriptor> planeDescriptors,
															Utils::BufferView<const vk::Image> planes,
															vk::ImageUsageFlags usage );

	static vk::UniqueImage				createImage(const Vulkan& vulkan,
													const PlaneDescriptor& desc, 
													vk::ImageUsageFlags usage,
													vk::ImageTiling tiling );
	static vk::UniqueImageView			createImageView(const Vulkan& vulkan,
														const PlaneDescriptor& desc,
														vk::Image image );
	static std::vector<Plane>			createPlanes(	const Vulkan& vulkan,
														Utils::BufferView<const PlaneDescriptor> planeDescriptors, 
														vk::ImageUsageFlags usage,
														vk::ImageTiling tiling );
	static Vulkan::AggregatedAllocation	createMemory(	const Vulkan& vulkan,
														const std::vector<Plane>& planes,
														vk::MemoryPropertyFlags memoryProperties );

};

}