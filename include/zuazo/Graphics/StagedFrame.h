#pragma once

#include "Vulkan.h"
#include "Frame.h"

namespace Zuazo::Graphics {

class StagedFrame 
	: public Frame 
{
public:
	using PixelData = Utils::BufferView<const Utils::BufferView<std::byte>>;
	using ConstPixelData = Utils::BufferView<const Utils::BufferView<const std::byte>>;

	StagedFrame(const Vulkan& vulkan,
				std::shared_ptr<const Descriptor> desc,
				std::shared_ptr<const Buffer> colorTransfer,
				Utils::BufferView<const PlaneDescriptor> planes,
				std::shared_ptr<const vk::UniqueCommandPool> cmdPool );
	StagedFrame(const StagedFrame& other) = delete;
	StagedFrame(StagedFrame&& other) noexcept = default;
	virtual ~StagedFrame(); 

	StagedFrame& 								operator=(const StagedFrame& other) noexcept = delete;

	PixelData									getPixelData() noexcept;
	ConstPixelData								getPixelData() const noexcept;
	void										flush();
	bool										waitCompletion(uint64_t timeo) const;

private:
	Buffer										m_stagingBuffer;
	std::vector<Utils::BufferView<std::byte>> 	m_pixelData;
		
	std::shared_ptr<const vk::UniqueCommandPool>m_commandPool;
	vk::UniqueCommandBuffer						m_commandBuffer;
	vk::SubmitInfo 								m_commandBufferSubmit;
	vk::UniqueFence								m_uploadComplete;

	static Buffer								createStagingBuffer(const Vulkan& vulkan,
																	Utils::BufferView<const Utils::Area> areas );
	static std::vector<Utils::BufferView<std::byte>> getPixelData(	const Vulkan& vulkan,
																	Utils::BufferView<const Utils::Area> areas,
																	const Buffer& buffer );
	static vk::UniqueCommandBuffer				createCommandBuffer(const Vulkan& vulkan,
																	Utils::BufferView<const PlaneDescriptor> planes,
																	vk::CommandPool cmdPool,
																	Utils::BufferView<const vk::UniqueImage> images,
																	Utils::BufferView<const Utils::Area> areas,
																	const Buffer& stagingBuffer );
	static vk::SubmitInfo						createSubmitInfo(const vk::CommandBuffer& cmdBuffer);
};



}