#pragma once

#include "Vulkan.h"
#include "TargetFrame.h"
#include "ColorTransfer.h"
#include "../Utils/Pimpl.h"
#include "../Utils/Limit.h"

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
	const Frame::Descriptor& 						getFrameDescriptor() const noexcept;
	OutputColorTransfer								getOutputColorTransfer() const;	

	void											setMaxSpareCount(size_t spares) noexcept;
	size_t											getMaxSpareCount() const noexcept;
	size_t											getSpareCount() const noexcept;

	std::shared_ptr<TargetFrame>					acquireFrame() const;

	static Utils::Discrete<ColorFormat> 			getSupportedFormats(const Vulkan& vulkan);
	static Utils::Discrete<ColorFormat> 			getSupportedSrgbFormats(const Vulkan& vulkan);
	static RenderPass								getRenderPass(	const Vulkan& vulkan, 
																	const Frame::Descriptor& frameDesc,
																	DepthStencilFormat depthStencilFmt );

private:
	struct Impl;
	Utils::Pimpl<Impl>								m_impl;

};

}