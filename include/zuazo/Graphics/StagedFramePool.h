#pragma once

#include "Vulkan.h"
#include "StagedFrame.h"
#include "../Utils/Pimpl.h"
#include "../Utils/Limit.h"

#include <memory>


namespace Zuazo::Graphics {

class StagedFramePool {
public:
	StagedFramePool(	const Vulkan& vulkan, 
				const Frame::Descriptor& desc );
	StagedFramePool(const StagedFramePool& other) = delete;
	StagedFramePool(StagedFramePool&& other) noexcept;
	~StagedFramePool();

	StagedFramePool& 								operator=(const StagedFramePool& other) = delete;
	StagedFramePool& 								operator=(StagedFramePool&& other) noexcept;

	const Vulkan&									getVulkan() const noexcept;
	const Frame::Descriptor& 						getFrameDescriptor() const noexcept;	

	void											setMaxSpareCount(size_t spares) noexcept;
	size_t											getMaxSpareCount() const noexcept;
	size_t											getSpareCount() const noexcept;

	std::shared_ptr<StagedFrame>					acquireFrame() const;

private:
	struct Impl;
	Utils::Pimpl<Impl>								m_impl;

};



}