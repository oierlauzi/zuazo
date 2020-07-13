#pragma once

#include "Vulkan.h"
#include "StagedFrame.h"
#include "ColorTransfer.h"
#include "Buffer.h"

#include "../Utils/Pool.h"

#include <memory>
#include <utility>

namespace Zuazo::Graphics {

class Uploader {
public:
	Uploader(	const Vulkan& vulkan, 
				const Frame::Descriptor& conf );
	Uploader(const Uploader& other) = delete;
	Uploader(Uploader&& other) = default;
	~Uploader() = default;

	Uploader& 										operator=(const Uploader& other) = delete;

	std::shared_ptr<StagedFrame>					acquireFrame() const;
	void											clear();

private:
	const Vulkan&									m_vulkan;
	std::shared_ptr<Frame::Descriptor>				m_frameDescriptor;
	ColorTransfer									m_colorTransfer;
	std::vector<Frame::PlaneDescriptor>				m_planeDescriptors;

	std::shared_ptr<vk::UniqueCommandPool>			m_commandPool;
	std::shared_ptr<Buffer>							m_colorTransferBuffer;

	Utils::Pool<StagedFrame>						m_pool;

	Utils::Pool<StagedFrame>::Ref					allocateFrame() const;

	static std::vector<Frame::PlaneDescriptor>		createPlaneDescriptors(	const Vulkan& vulkan, 
																			const Frame::Descriptor& desc,
																			ColorTransfer& colorTransfer );
	static std::shared_ptr<vk::UniqueCommandPool> 	createCommandPool(const Vulkan& vulkan);
};

}