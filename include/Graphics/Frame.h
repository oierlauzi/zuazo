#pragma once

#include "Vulkan.h"
#include "ImageProperties.h"

namespace Zuazo::Graphics {

class Frame {
public:
	Frame() = default;
	Frame(	const Vulkan& vulkan, 
			const ImageProperties& properties,
			vk::Image image );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	~Frame() = default;

	Frame& 						operator=(const Frame& other) = delete;
	Frame& 						operator=(Frame&& other) = default;

private:
	using Samplers = std::array<vk::Sampler, VK_FILTER_RANGE_SIZE>;

	ImageProperties				m_properties;

	vk::Image					m_image;
	vk::UniqueImageView			m_imageView;

	Samplers					m_samplers;

	static vk::UniqueImageView	createImageView(	const Vulkan& vulkan,
													vk::Format format,
													const vk::ComponentMapping& swizzle,
													vk::Image image );
	static Samplers				getSamplers(const Vulkan& vulkan,
											const ImageProperties& properties );


};


}