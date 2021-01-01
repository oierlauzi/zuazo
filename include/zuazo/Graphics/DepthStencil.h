#pragma once

#include "Vulkan.h"
#include "Image.h"

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
	vk::ImageView					getImageView() const noexcept;
	vk::DeviceMemory				getDeviceMemory() const noexcept;

private:
	Image							m_image;

	static Image 					createImage(const Vulkan& vulkan, 
												vk::Extent2D ext,
												vk::Format fmt );
};

}