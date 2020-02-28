#pragma once

#include "Vulkan.h"
#include "Frame.h"

#include "../Resolution.h"
#include "../ColorSubsampling.h"
#include "../ColorFormat.h"
#include "../ColorRange.h"
#include "../ColorTransferFunction.h"
#include "../ColorModel.h"
#include "../ColorPrimaries.h"

#include <memory>
#include <utility>
#include <span>

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

	using PixelData = std::array<std::span<std::byte>, IMAGE_COUNT>;

	Uploader(	const Vulkan& vulkan, 
				const Descriptor& descriptor );
	Uploader(const Uploader& other) = delete;
	Uploader(Uploader&& other) = default;
	~Uploader() = default;

	Uploader& 									operator=(const Uploader& other) = delete;

	std::tuple<const std::shared_ptr<Frame>&, const PixelData&> acquireFrame() const;
	void										flush(const std::shared_ptr<Frame>& frame) const;

	static Descriptor							getDescriptor(	const Vulkan& vulkan,
																Resolution resolution,
																ColorSubsampling subsampling,
																ColorFormat format,
																ColorRange range,
																ColorTransferFunction transferFunction,
																ColorModel model,
																ColorPrimaries primaries );
private:
	struct FrameData : Frame::Data {
		Buffer stagingBuffer;
		Buffer::MappedMemory mappedMemory;
		PixelData pixelData;
		 
		std::shared_ptr<vk::UniqueCommandPool> commandPool;
		vk::UniqueCommandBuffer	commandBuffer;
		vk::SubmitInfo commandBufferSubmit;
	};

	const Vulkan& 								m_vulkan;
	Descriptor									m_descriptor;

	mutable std::vector<std::shared_ptr<Frame>>	m_frames;
	std::shared_ptr<vk::UniqueCommandPool>		m_commandPool;

	std::shared_ptr<Frame>						createFrame() const;

	static std::shared_ptr<vk::UniqueCommandPool> createCommandPool(const Vulkan& vulkan);
};


}