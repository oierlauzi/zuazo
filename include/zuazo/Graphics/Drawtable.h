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
	Drawtable(Drawtable&& other);
	~Drawtable();

	Drawtable& 										operator=(const Drawtable& other) = delete;
	Drawtable& 										operator=(Drawtable&& other);

	const Vulkan&									getVulkan() const;

	void											setMaxSpareCount(size_t spares);
	size_t											getMaxSpareCount() const;
	size_t											getSpareCount() const;

	std::shared_ptr<TargetFrame>					acquireFrame() const;
	OutputColorTransfer								getOutputColorTransfer() const;

private:
	struct Impl;
	Utils::Pimpl<Impl>								m_impl;

};



}