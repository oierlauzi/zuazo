#pragma once

#include "Vulkan.h"

namespace Zuazo::Graphics {

class DepthStencil {
public:
	DepthStencil(	const Vulkan& vulkan,
					vk::Extent2D ext,
					vk::Format fmt );
	DepthStencil(const DepthStencil& other) = delete;
	DepthStencil(DepthStencil&& other) noexcept = default;
	~DepthStencil() = default;

	DepthStencil&					operator=(const DepthStencil& other) = delete;
	DepthStencil&					operator=(DepthStencil&& other) noexcept = default;

	vk::Image						getImage() const noexcept;
	vk::DeviceMemory				getDeviceMemory() const noexcept;
	vk::ImageView					getImageView() const noexcept;

private:
	vk::UniqueImage					m_image;
	vk::UniqueDeviceMemory			m_memory;
	vk::UniqueImageView				m_imageView;

	static vk::UniqueImage			createImage(const Vulkan& vulkan, 
												vk::Extent2D ext,
												vk::Format fmt );
	static vk::UniqueDeviceMemory 	createDeviceMemory(	const Vulkan& vulkan, 
														vk::Image image );
	static vk::UniqueImageView 		createImageView(const Vulkan& vulkan, 
													vk::Format fmt, 
													vk::Image image );
};

}