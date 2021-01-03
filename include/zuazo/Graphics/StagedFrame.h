#pragma once

#include "Vulkan.h"
#include "Frame.h"
#include "Image.h"

namespace Zuazo::Graphics {

class StagedFrame 
	: public Frame 
{
public:
	using PixelData = Utils::BufferView<const Utils::BufferView<std::byte>>;
	using ConstPixelData = Utils::BufferView<const Utils::BufferView<const std::byte>>;

	StagedFrame(const Vulkan& vulkan,
				std::shared_ptr<const Descriptor> desc,
				std::shared_ptr<const InputColorTransfer> colorTransfer,
				std::shared_ptr<const Buffer> colorTransferBuffer,
				Utils::BufferView<const Image::PlaneDescriptor> planes,
				std::shared_ptr<const vk::UniqueCommandPool> cmdPool );
	StagedFrame(const StagedFrame& other) = delete;
	StagedFrame(StagedFrame&& other) noexcept = default;
	virtual ~StagedFrame(); 

	StagedFrame& 								operator=(const StagedFrame& other) noexcept = delete;

	PixelData									getPixelData() noexcept;
	ConstPixelData								getPixelData() const noexcept;
	void										flush();
	bool										waitCompletion(uint64_t timeo) const;

	static std::shared_ptr<vk::UniqueCommandPool> createCommandPool(const Vulkan& vulkan);

private:
	Image										m_stagingImage;
	std::vector<Utils::BufferView<std::byte>> 	m_pixelData;
		
	std::shared_ptr<const vk::UniqueCommandPool>m_commandPool;
	vk::UniqueCommandBuffer						m_commandBuffer;
	vk::SubmitInfo 								m_commandBufferSubmit;
	vk::UniqueFence								m_uploadComplete;

	void										transitionStagingimageLayout();
	void										recordCommandBuffer(Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors);

	static Image								createStagingImage(	const Vulkan& vulkan,
																	Utils::BufferView<const Image::PlaneDescriptor> planes );
	static std::vector<Utils::BufferView<std::byte>> getPixelData(	const Vulkan& vulkan,
																	const Image& stagingImage );
	static vk::UniqueCommandBuffer				createCommandBuffer(const Vulkan& vulkan,
																	vk::CommandPool cmdPool );
	static vk::SubmitInfo						createSubmitInfo(const vk::CommandBuffer& cmdBuffer);

};



}