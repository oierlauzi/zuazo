#pragma once

#include "Vulkan.h"
#include "ColorTransfer.h"
#include "Buffer.h"
#include "Image.h"

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
		using Extents = std::array<vk::Extent2D, IMAGE_COUNT>;
		using Formats = std::array<std::tuple<vk::Format, vk::ComponentMapping>, IMAGE_COUNT>;

		Extents extents;
		Formats colorFormat;
		ColorTransfer colorTransfer;
	};

	Frame() = default;
	Frame(	const Vulkan& vulkan, 
			Image&& image,
			std::shared_ptr<Buffer>&& colorTransfer );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	~Frame();

	Frame& 							operator=(const Frame& other) = delete;
	Frame& 							operator=(Frame&& other) = default;

	void							bind(	vk::CommandBuffer cmd,
											vk::PipelineLayout layout,
											uint32_t index );

	static Descriptor				getDescriptorForUpload(	const Vulkan& vulkan,
															Resolution resolution,
															ColorSubsampling subsampling,
															ColorFormat format,
															ColorRange range,
															ColorTransferFunction transferFunction,
															ColorModel model,
															ColorPrimaries primaries );
private:
	static constexpr size_t DESCRIPTOR_COUNT = VK_FILTER_RANGE_SIZE;
	using DescriptorSets = std::array<vk::DescriptorSet, DESCRIPTOR_COUNT>;

	const Vulkan&					m_vulkan;

	Image 							m_image;

	std::shared_ptr<Buffer> 		m_colorTransfer;	
	vk::UniqueDescriptorPool		m_descriptorPool;
	DescriptorSets					m_descriptorSets;

	vk::UniqueDescriptorPool		createDescriptorPool(const Vulkan& vulkan);
	DescriptorSets					allocateDescriptorSets(	const Vulkan& vulkan,
															vk::DescriptorPool pool );
};

}