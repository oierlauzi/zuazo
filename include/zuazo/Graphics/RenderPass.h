#pragma once

#include "Vulkan.h"
#include "Image.h"
#include "ColorTransfer.h"
#include "../Utils/Pimpl.h"

namespace Zuazo::Graphics {

class RenderPass {
public:
	RenderPass();
	RenderPass(	const Vulkan& vulkan, 
				const ColorTransferWrite& colorTransfer,
				Utils::BufferView<const Image::Plane> planeDescriptors,
				DepthStencilFormat depthStencilFmt,
				vk::ImageLayout finalLayout );

	RenderPass(const RenderPass& other) = delete;
	RenderPass(RenderPass&& other) noexcept;
	~RenderPass();

	RenderPass& 						operator=(const RenderPass& other) = delete;
	RenderPass& 						operator=(RenderPass&& other) noexcept;

	vk::RenderPass						get() const noexcept;


	vk::UniqueFramebuffer				createFramebuffer(	const Vulkan& vulkan, 
															const Image& target) const;
	void								finalize(	const Vulkan& vulkan, 
													vk::CommandBuffer cmd ) const noexcept;

	static Utils::BufferView<const vk::ClearValue> getClearValues(DepthStencilFormat depthStencilFmt);

	static const RenderPass NO_RENDERPASS;

private:
	struct Impl;
	Utils::Pimpl<Impl>					m_impl;

};

}