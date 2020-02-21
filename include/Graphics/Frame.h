#pragma once

#include "Vulkan.h"
#include "ColorTransfer.h"

namespace Zuazo::Graphics {

class Frame {
public:
	using Formats = std::array<std::tuple<vk::Format, vk::ComponentMapping>, Vulkan::SAMPLER_COUNT>;
	using Images = std::array<vk::Image, Vulkan::SAMPLER_COUNT>;

	Frame() = default;
	Frame(	const Vulkan& vulkan, 
			const Formats& formats,
			const ColorTransfer& colorTransfer,
			const Images& images );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	~Frame();

	Frame& 						operator=(const Frame& other) = delete;
	Frame& 						operator=(Frame&& other) = default;

private:
	using ImageViews = std::array<vk::UniqueImageView, Vulkan::SAMPLER_COUNT>; 

	const Vulkan&				m_vulkan;

	Images						m_images;
	ImageViews					m_imageViews;

	static ImageViews			createImageViews(	const Vulkan& vulkan,
													const Formats& formats,
													const Images& images );

};


}