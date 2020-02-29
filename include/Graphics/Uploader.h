#pragma once

#include "Vulkan.h"
#include "Frame.h"
#include "Buffer.h"
#include "MappedMemory.h"

#include "../Resolution.h"
#include "../ColorSubsampling.h"
#include "../ColorFormat.h"
#include "../ColorRange.h"
#include "../ColorTransferFunction.h"
#include "../ColorModel.h"
#include "../ColorPrimaries.h"

#include <memory>
#include <utility>

namespace Zuazo::Graphics {

class Uploader {
public:
	struct Descriptor {
		using Extents = std::array<vk::Extent2D, IMAGE_COUNT>;
		using Formats = std::array<std::tuple<vk::Format, vk::ComponentMapping>, IMAGE_COUNT>;

		Extents extents;
		Formats colorFormat;
		ColorTransfer colorTransfer;
	};

	class Frame;

	Uploader(	const Vulkan& vulkan, 
				const Descriptor& descriptor );
	Uploader(const Uploader& other) = delete;
	Uploader(Uploader&& other) = default;
	~Uploader() = default;

	Uploader& 									operator=(const Uploader& other) = delete;

	const std::shared_ptr<Frame>&				acquireFrame() const;

	static Descriptor							getDescriptor(	const Vulkan& vulkan,
																Resolution resolution,
																ColorSubsampling subsampling,
																ColorFormat format,
																ColorRange range,
																ColorTransferFunction transferFunction,
																ColorModel model,
																ColorPrimaries primaries );
private:
	const Vulkan& 								m_vulkan;
	Descriptor									m_descriptor;

	mutable std::vector<std::shared_ptr<Frame>>	m_frames;

	std::shared_ptr<vk::UniqueCommandPool>		m_commandPool;
	std::shared_ptr<Buffer>						m_colorTransferBuffer;

	const std::shared_ptr<Frame>&				getUniqueFrame() const;

	static std::shared_ptr<vk::UniqueCommandPool> createCommandPool(const Vulkan& vulkan);
};

class Uploader::Frame : public Graphics::Frame {
public:
	Frame(	const Vulkan& vulkan,
			const Descriptor& desc,
			std::shared_ptr<const Buffer>&& colorTransfer,
			std::shared_ptr<const vk::UniqueCommandPool>&& cmdPool );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	virtual ~Frame() = default; 

	Frame& 										operator=(const Frame& other) = delete;

	const PixelData&							getPixelData();
	void										flush();
private:
	Buffer										m_stagingBuffer;
	MappedMemory 								m_stagingBufferMemory;
	PixelData 									m_pixelData;
		
	std::shared_ptr<const vk::UniqueCommandPool>m_commandPool;
	vk::UniqueCommandBuffer						m_commandBuffer;
	vk::SubmitInfo 								m_commandBufferSubmit;

	static Image								createImage(const Vulkan& vulkan,
															const Descriptor& desc );
	static Buffer								createStagingBuffer(const Vulkan& vulkan,
																	const Image& image );
	static MappedMemory							mapStagingBuffer(	const Vulkan& vulkan,
																	const Buffer& buffer);
	static PixelData							createPixelData(const Image& image,
																MappedMemory& memory );
	static vk::UniqueCommandBuffer				createCommandBuffer(const Vulkan& vulkan,
																	const Descriptor& desc,
																	vk::CommandPool cmdPool,
																	const Image& image,
																	const Buffer& stagingBuffer );
	static vk::SubmitInfo						createSubmitInfo(const vk::CommandBuffer& cmdBuffer);
};



}