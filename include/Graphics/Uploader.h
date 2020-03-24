#pragma once

#include "Vulkan.h"
#include "Frame.h"
#include "Buffer.h"

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
	class Frame;

	Uploader(	const Vulkan& vulkan, 
				const Graphics::Frame::Descriptor& descriptor );
	Uploader(const Uploader& other) = delete;
	Uploader(Uploader&& other) = default;
	~Uploader() = default;

	Uploader& 									operator=(const Uploader& other) = delete;

	const std::shared_ptr<Frame>&				acquireFrame() const;
	void										clear();

	static Graphics::Frame::Descriptor			getDescriptor(	const Vulkan& vulkan,
																Resolution resolution,
																ColorSubsampling subsampling,
																ColorFormat format,
																ColorRange range,
																ColorTransferFunction transferFunction,
																ColorModel model,
																ColorPrimaries primaries );
private:
	const Vulkan& 								m_vulkan;
	Graphics::Frame::Descriptor					m_descriptor;

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
			const std::shared_ptr<const Buffer>& colorTransfer,
			const std::shared_ptr<const vk::UniqueCommandPool>& cmdPool );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	virtual ~Frame() = default; 

	Frame& 										operator=(const Frame& other) = delete;

	const PixelData&							getPixelData();
	void										flush();
private:
	Buffer										m_stagingBuffer;
	PixelData 									m_pixelData;
		
	std::shared_ptr<const vk::UniqueCommandPool>m_commandPool;
	vk::UniqueCommandBuffer						m_commandBuffer;
	vk::SubmitInfo 								m_commandBufferSubmit;

	static Buffer								createStagingBuffer(const Vulkan& vulkan,
																	const Image& image );
	static PixelData							getPixelData(	const Vulkan& vulkan,
																const Image& image,
																const Buffer& buffer );
	static vk::UniqueCommandBuffer				createCommandBuffer(const Vulkan& vulkan,
																	const Descriptor& desc,
																	vk::CommandPool cmdPool,
																	const Image& image,
																	const Buffer& stagingBuffer );
	static vk::SubmitInfo						createSubmitInfo(const vk::CommandBuffer& cmdBuffer);
};



}