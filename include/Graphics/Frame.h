#pragma once

#include "Vulkan.h"
#include "ColorTransfer.h"
#include "Buffer.h"
#include "Image.h"

#include <memory>

namespace Zuazo::Graphics {

class Frame {
public:	
	struct Data { 
		virtual ~Data() = default;
	};

	Frame(	const Vulkan& vulkan,
			Image&& image,
			std::shared_ptr<const Buffer>&& colorTransfer,
			std::unique_ptr<Data>&& data );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	~Frame();

	Frame& 							operator=(const Frame& other) = delete;

	vk::Semaphore&					getReadySemaphore();
	const vk::Semaphore&			getReadySemaphore() const;

	vk::Fence&						getReadyFence();
	const vk::Fence&				getReadyFence() const;

	Image&							getImage();
	const Image&					getImage() const;

	const Buffer&					getColorTransfer() const;

	Data&							getData();
	const Data&						getData() const;

	void							bind(	vk::CommandBuffer cmd,
											vk::PipelineBindPoint bindPoint,
											vk::PipelineLayout layout,
											uint32_t index,
											vk::Filter filter );
											
private:
	static constexpr size_t DESCRIPTOR_COUNT = VK_FILTER_RANGE_SIZE;
	using DescriptorSets = std::array<vk::DescriptorSet, DESCRIPTOR_COUNT>;

	const Vulkan&					m_vulkan;

	vk::UniqueSemaphore				m_readySemaphore;
	vk::UniqueFence					m_readyFence;

	Image							m_image;
	std::shared_ptr<const Buffer>	m_colorTransfer;
	std::unique_ptr<Data>			m_data;	

	vk::UniqueDescriptorPool		m_descriptorPool;
	DescriptorSets					m_descriptorSets;

	vk::UniqueDescriptorPool		createDescriptorPool(const Vulkan& vulkan);
	DescriptorSets					allocateDescriptorSets(	const Vulkan& vulkan,
															vk::DescriptorPool pool );
};

}