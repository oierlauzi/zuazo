#pragma once

#include "Vulkan.h"
#include "ColorTransfer.h"

namespace Zuazo::Graphics {

class Frame {
public:	
	using Formats = std::array<std::tuple<vk::Format, vk::ComponentMapping>, SAMPLER_COUNT>;
	using Images = std::array<std::tuple<vk::UniqueImage, vk::UniqueDeviceMemory>, SAMPLER_COUNT>;
	using ImageViews = std::array<vk::UniqueImageView, SAMPLER_COUNT>; 

	Frame() = default;
	Frame(	const Vulkan& vulkan, 
			const Formats& formats,
			const ColorTransfer& colorTransfer,
			std::shared_ptr<Images>&& images );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	~Frame();

	Frame& 							operator=(const Frame& other) = delete;
	Frame& 							operator=(Frame&& other) = default;

	const ColorTransfer& 			getColorTransfer() const;
	const Images&					getImages() const;
	const ImageViews&				getImageViews() const;

private:
	const Vulkan&					m_vulkan;

	ColorTransfer					m_colorTransfer;

	std::shared_ptr<Images>			m_images;
	ImageViews						m_imageViews;

	static ImageViews				createImageViews(	const Vulkan& vulkan,
														const Formats& formats,
														const Images& images );

	static_assert(Vulkan::SAMPLER_COUNT >= SAMPLER_COUNT);
};


}