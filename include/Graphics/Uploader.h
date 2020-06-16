#pragma once

#include "Vulkan.h"
#include "StagedFrame.h"
#include "ColorTransfer.h"
#include "Buffer.h"

#include "../Utils/Pool.h"

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

	Uploader& 										operator=(const Uploader& other) = delete;

	std::shared_ptr<StagedFrame>					acquireFrame() const;
	void											clear();

private:
	const Vulkan&									m_vulkan;
	Math::Vec2f										m_frameSize;
	ColorTransfer									m_colorTransfer;
	std::vector<Frame::PlaneDescriptor>				m_planeDescriptors;

	std::shared_ptr<vk::UniqueCommandPool>			m_commandPool;
	std::shared_ptr<Buffer>							m_colorTransferBuffer;

	Utils::Pool<StagedFrame>						m_pool;

	Utils::Pool<StagedFrame>::Ref					allocateFrame() const;

	static std::vector<Frame::PlaneDescriptor>		createPlaneDescriptors(	const Vulkan& vulkan, 
																			Resolution resolution,
																			ColorSubsampling subsampling,
																			ColorFormat format,
																			ColorTransfer& colorTransfer );
	static std::shared_ptr<vk::UniqueCommandPool> 	createCommandPool(const Vulkan& vulkan);
};

}