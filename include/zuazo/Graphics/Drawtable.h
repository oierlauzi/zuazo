#pragma once

#include "Vulkan.h"
#include "TargetFrame.h"
#include "ColorTransfer.h"
#include "../Utils/Pimpl.h"

#include <memory>
#include <vector>


namespace Zuazo::Graphics {

class Drawtable {
public:
	Drawtable(	const Vulkan& vulkan, 
				const Frame::Descriptor& frameDesc,
				DepthStencilFormat depthStencilFmt );
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

	static std::vector<ColorFormat> 				getSupportedFormats(const Vulkan& vulkan);
	static std::vector<DepthStencilFormat> 			getSupportedFormatsDepthStencil(const Vulkan& vulkan);
	static vk::RenderPass							getRenderPass(	const Vulkan& vulkan, 
																	const Frame::Descriptor& frameDesc,
																	DepthStencilFormat depthStencilFmt );
private:
	struct Impl;
	Utils::Pimpl<Impl>								m_impl;

};

}