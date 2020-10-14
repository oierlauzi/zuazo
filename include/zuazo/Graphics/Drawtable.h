#pragma once

#include "Vulkan.h"
#include "TargetFrame.h"
#include "ColorTransfer.h"
#include "../Utils/Pimpl.h"

#include <memory>


namespace Zuazo::Graphics {

class Drawtable {
public:
	Drawtable(	const Vulkan& vulkan, 
				const Frame::Descriptor& conf,
				std::shared_ptr<const vk::UniqueRenderPass> renderPass,
				vk::Format depthStencilFmt );
	Drawtable(const Drawtable& other) = delete;
	Drawtable(Drawtable&& other) noexcept;
	~Drawtable();

	Drawtable& 										operator=(const Drawtable& other) = delete;
	Drawtable& 										operator=(Drawtable&& other) noexcept;

	const Vulkan&									getVulkan() const noexcept;

	void											setMaxSpareCount(size_t spares) noexcept;
	size_t											getMaxSpareCount() const noexcept;
	size_t											getSpareCount() const noexcept;

	std::shared_ptr<TargetFrame>					acquireFrame() const;
	OutputColorTransfer								getOutputColorTransfer() const;

private:
	struct Impl;
	Utils::Pimpl<Impl>								m_impl;

};



}