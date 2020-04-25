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
				Resolution resolution,
				AspectRatio par,
				ColorSubsampling subsampling,
				ColorFormat format,
				ColorRange range,
				ColorTransferFunction transferFunction,
				ColorModel model,
				ColorPrimaries primaries );
	Uploader(const Uploader& other) = delete;
	Uploader(Uploader&& other) = default;
	~Uploader() = default;

	Uploader& 									operator=(const Uploader& other) = delete;

	const std::shared_ptr<Frame>&				acquireFrame() const;
	void										clear();

private:
	const Vulkan& 								m_vulkan;

	Math::Vec2f									m_frameSize;
	ColorTransfer								m_colorTransfer;
	std::vector<Image::PlaneDescriptor>			m_planeDescriptors;

	mutable std::vector<std::shared_ptr<Frame>>	m_frames;

	std::shared_ptr<vk::UniqueCommandPool>		m_commandPool;
	std::shared_ptr<Buffer>						m_colorTransferBuffer;

	const std::shared_ptr<Frame>&				getUniqueFrame() const;

	static std::vector<Image::PlaneDescriptor>	createPlaneDescriptors(	const Vulkan& vulkan, 
																		Resolution resolution,
																		ColorSubsampling subsampling,
																		ColorFormat format, 
																		ColorTransfer& colorTransfer);
	static std::shared_ptr<vk::UniqueCommandPool> createCommandPool(const Vulkan& vulkan);
};

class Uploader::Frame : public Graphics::Frame {
public:
	Frame(	const Vulkan& vulkan,
			Math::Vec2f size,
			Utils::BufferView<const Image::PlaneDescriptor> planes,
			const std::shared_ptr<const Buffer>& colorTransfer,
			const std::shared_ptr<const vk::UniqueCommandPool>& cmdPool );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	virtual ~Frame(); 

	Frame& 										operator=(const Frame& other) = delete;

	const Image::PixelData &					getPixelData();
	void										flush();
private:
	Buffer										m_stagingBuffer;
	Image::PixelData 							m_pixelData;
		
	std::shared_ptr<const vk::UniqueCommandPool>m_commandPool;
	vk::UniqueCommandBuffer						m_commandBuffer;
	vk::SubmitInfo 								m_commandBufferSubmit;

	static Buffer								createStagingBuffer(const Vulkan& vulkan,
																	const Image& image );
	static Image::PixelData 					getPixelData(	const Vulkan& vulkan,
																const Image& image,
																const Buffer& buffer );
	static vk::UniqueCommandBuffer				createCommandBuffer(const Vulkan& vulkan,
																	Utils::BufferView<const Image::PlaneDescriptor> planes,
																	vk::CommandPool cmdPool,
																	const Image& image,
																	const Buffer& stagingBuffer );
	static vk::SubmitInfo						createSubmitInfo(const vk::CommandBuffer& cmdBuffer);
};



}