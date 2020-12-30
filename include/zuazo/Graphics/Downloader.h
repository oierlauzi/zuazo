#pragma once

#include "Vulkan.h"
#include "TargetFrame.h"
#include "ColorTransfer.h"
#include "../Utils/Pimpl.h"
#include "../Utils/Limit.h"

#include <memory>
#include <vector>


namespace Zuazo::Graphics {

class Downloader {
public:
	using PixelData = Utils::BufferView<const Utils::BufferView<const std::byte>>;

	Downloader(	const Vulkan& vulkan, 
				const Frame::Descriptor& frameDesc,
				DepthStencilFormat depthStencilFmt );
	Downloader(const Downloader& other) = delete;
	Downloader(Downloader&& other) noexcept;
	~Downloader();

	Downloader& 									operator=(const Downloader& other) = delete;
	Downloader& 									operator=(Downloader&& other) noexcept;

	const Vulkan&									getVulkan() const noexcept;
	const Frame::Descriptor& 						getFrameDescriptor() const noexcept;	
	OutputColorTransfer								getOutputColorTransfer() const;

	vk::Framebuffer									getFramebuffer() const noexcept;
	void											beginRenderPass(vk::CommandBuffer cmd, 
																	vk::Rect2D renderArea,
																	Utils::BufferView<const vk::ClearValue> clearValues,
																	vk::SubpassContents contents ) const noexcept;
	void											endRenderPass(vk::CommandBuffer cmd) const noexcept;

	void											draw(std::shared_ptr<const CommandBuffer> cmd);
	bool											waitCompletion(uint64_t timeo) const;
	PixelData										getPixelData() const noexcept;

	static Utils::Discrete<ColorFormat> 			getSupportedFormats(const Vulkan& vulkan);
	static Utils::Discrete<ColorFormat> 			getSupportedSrgbFormats(const Vulkan& vulkan);
	static Utils::Discrete<DepthStencilFormat> 		getSupportedFormatsDepthStencil(const Vulkan& vulkan);
	static RenderPass								getRenderPass(	const Vulkan& vulkan, 
																	const Frame::Descriptor& frameDesc,
																	DepthStencilFormat depthStencilFmt );
	static std::vector<vk::ClearValue> 				getClearValues(	const Graphics::Frame::Descriptor& frameDesc,
																	DepthStencilFormat depthStencilFmt );

private:
	struct Impl;
	Utils::Pimpl<Impl>								m_impl;

};

}