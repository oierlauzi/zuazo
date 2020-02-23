#pragma once

#include "Vulkan.h"
#include "ColorTransfer.h"

#include "../Resolution.h"
#include "../ColorSubsampling.h"
#include "../ColorFormat.h"
#include "../ColorRange.h"
#include "../ColorTransferFunction.h"
#include "../ColorModel.h"
#include "../ColorPrimaries.h"

namespace Zuazo::Graphics {

class Frame {
public:	
	struct Descriptor {
		using Extents = std::array<vk::Extent2D, SAMPLER_COUNT>;
		using Formats = std::array<std::tuple<vk::Format, vk::ComponentMapping>, SAMPLER_COUNT>;

		Extents extents;
		Formats colorFormat;
		ColorTransfer colorTransfer;
	};

	using Images = std::array<std::tuple<vk::UniqueImage, vk::UniqueDeviceMemory>, SAMPLER_COUNT>;
	using ImageViews = std::array<vk::UniqueImageView, SAMPLER_COUNT>; 

	Frame() = default;
	Frame(	const Vulkan& vulkan, 
			const Descriptor& desc,
			std::shared_ptr<Images>&& images );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	~Frame();

	Frame& 							operator=(const Frame& other) = delete;
	Frame& 							operator=(Frame&& other) = default;

	const ColorTransfer& 			getColorTransfer() const;
	const Images&					getImages() const;
	const ImageViews&				getImageViews() const;

	static Descriptor				getDescriptorForUpload(	const Vulkan& vulkan,
															Resolution resolution,
															ColorSubsampling subsampling,
															ColorFormat format,
															ColorRange range,
															ColorTransferFunction transferFunction,
															ColorModel model,
															ColorPrimaries primaries );
private:
	static constexpr size_t DESCRIPTOR_SET_COUNT = VK_FILTER_RANGE_SIZE;
	using DescriptorSets = std::array<vk::DescriptorSet, DESCRIPTOR_SET_COUNT>;

	const Vulkan&					m_vulkan;

	ColorTransfer					m_colorTransfer;

	std::shared_ptr<Images>			m_images;
	ImageViews						m_imageViews;
	
	vk::UniqueDescriptorPool		m_descriptorPool;
	DescriptorSets					m_descriptorSets;	

	static ImageViews				createImageViews(	const Vulkan& vulkan,
														const Descriptor::Formats& formats,
														const Images& images );

	vk::UniqueDescriptorPool		createDescriptorPool(const Vulkan& vulkan);
	DescriptorSets					allocateDescriptorSets(	const Vulkan& vulkan,
															vk::DescriptorPool pool );
};

}