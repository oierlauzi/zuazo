#pragma once

#include "Vulkan.h"
#include "TargetFrame.h"
#include "ColorTransfer.h"
#include "../Utils/Pimpl.h"
#include "../Utils/Limit.h"

#include <memory>
#include <vector>


namespace Zuazo::Graphics {

class TargetFramePool {
public:
	TargetFramePool(const Vulkan& vulkan, 
					const Frame::Descriptor& frameDesc,
					DepthStencilFormat depthStencilFmt );
	TargetFramePool(const TargetFramePool& other) = delete;
	TargetFramePool(TargetFramePool&& other) noexcept;
	~TargetFramePool();

	TargetFramePool& 								operator=(const TargetFramePool& other) = delete;
	TargetFramePool& 								operator=(TargetFramePool&& other) noexcept;

	const Vulkan&									getVulkan() const noexcept;
	const Frame::Descriptor& 						getFrameDescriptor() const noexcept;
	DepthStencilFormat								getDepthStencilFormat() const noexcept;

	void											setMaxSpareCount(size_t spares) noexcept;
	size_t											getMaxSpareCount() const noexcept;
	size_t											getSpareCount() const noexcept;

	std::shared_ptr<TargetFrame>					acquireFrame() const;

	const RenderPass&								getRenderPass() const noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>								m_impl;

};

}