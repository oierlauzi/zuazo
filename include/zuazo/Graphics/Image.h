#pragma once

#include "Vulkan.h"
#include "../Utils/BufferView.h"

#include <vector>

namespace Zuazo::Graphics {

class Image {
public:
	class Plane;

	Image() = default;
	Image(	const Vulkan& vulkan,
			Utils::BufferView<const Plane> planes,
			vk::ImageUsageFlags usage,
			vk::ImageTiling tiling,
			vk::MemoryPropertyFlags memoryProp );
	Image(const Image& other) = delete;
	Image(Image&& other) = default;
	~Image() = default;

	Image&								operator=(const Image& other) = delete;
	Image&								operator=(Image&& other) = default;

	Utils::BufferView<const Plane>		getPlanes() const;
	const Vulkan::AggregatedAllocation&	getMemory() const;

private:
	std::vector<Plane>					m_planes;
	std::vector<vk::UniqueImage>		m_images;
	std::vector<vk::UniqueImageView>	m_imageViews;
	Vulkan::AggregatedAllocation		m_memory;


	void								createImages(	const Vulkan& vulkan,
														vk::ImageUsageFlags usage,
														vk::ImageTiling tiling,
														vk::MemoryPropertyFlags memoryProp );
	void								createImageViews(	const Vulkan& vulkan,
															vk::ImageUsageFlags usage );

	static vk::UniqueImage				createImage(const Vulkan& vulkan,
													Plane& plane, 
													vk::ImageUsageFlags usage,
													vk::ImageTiling tiling );
	static vk::UniqueImageView			createImageView(const Vulkan& vulkan,
														Plane& plane );

};

class Image::Plane {
public:
	Plane(	vk::Extent3D extent,
			vk::Format format,
			const vk::ComponentMapping& swizzle = vk::ComponentMapping(),
			vk::Image image = vk::Image(),
			vk::ImageView imageView = vk::ImageView() ) noexcept;

	Plane(const Plane& other) = default;
	~Plane() = default;

	Plane&								operator=(const Plane& other) = default;

	void								setExtent(vk::Extent3D extent) noexcept;
	vk::Extent3D						getExtent() const noexcept;

	void								setFormat(vk::Format format) noexcept;
	vk::Format							getFormat() const noexcept;

	void								setSwizzle(const vk::ComponentMapping& swizzle) noexcept;
	const vk::ComponentMapping&			getSwizzle() const noexcept;

	void								setImage(vk::Image image) noexcept;
	vk::Image							getImage() const noexcept;

	void								setImageView(vk::ImageView imageView) noexcept;
	vk::ImageView						getImageView() const noexcept;

private:
	vk::Extent3D						m_extent;
	vk::Format							m_format;
	vk::ComponentMapping				m_swizzle;
	vk::Image							m_image;
	vk::ImageView						m_imageView;

};



void copy(	const Vulkan& vulkan,
			vk::CommandBuffer cmd, 
			const Image& src, 
			Image& dst );

}