#pragma once

#include "Vulkan.h"
#include "ColorTransfer.h"
#include "Buffer.h"
#include "Image.h"

#include <memory>
#include <span>

namespace Zuazo::Graphics {

class Frame {
public:	
	using PixelData = std::array<std::span<std::byte>, IMAGE_COUNT>;

	Frame(	const Vulkan& vulkan,
			Image&& image,
			std::shared_ptr<const Buffer>&& colorTransfer );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	virtual ~Frame();

	Frame& 							operator=(const Frame& other) = delete;

	vk::Fence&						getReadyFence();
	const vk::Fence&				getReadyFence() const;

	Image&							getImage();
	const Image&					getImage() const;

	const Buffer&					getColorTransfer() const;

	void							bind(	vk::CommandBuffer cmd,
											vk::PipelineBindPoint bindPoint,
											vk::PipelineLayout layout,
											uint32_t index,
											vk::Filter filter );

	static std::shared_ptr<Buffer> 	createColorTransferBuffer(	const Vulkan& vulkan,
																const ColorTransfer& colorTransfer );

protected:
	const Vulkan&					m_vulkan;
	
private:
	static constexpr size_t DESCRIPTOR_COUNT = VK_FILTER_RANGE_SIZE;
	using DescriptorSets = std::array<vk::DescriptorSet, DESCRIPTOR_COUNT>;

	vk::UniqueFence					m_readyFence;

	Image							m_image;
	std::shared_ptr<const Buffer>	m_colorTransfer;

	vk::UniqueDescriptorPool		m_descriptorPool;
	DescriptorSets					m_descriptorSets;

	vk::UniqueDescriptorPool		createDescriptorPool(const Vulkan& vulkan);
	DescriptorSets					allocateDescriptorSets(	const Vulkan& vulkan,
															vk::DescriptorPool pool );
};

}