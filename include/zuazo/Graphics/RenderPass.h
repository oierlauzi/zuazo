#pragma once

#include "Vulkan.h"
#include "Frame.h"

namespace Zuazo::Graphics {

class RenderPass {
public:
	RenderPass() = default;
	RenderPass(	const Vulkan& vulkan, 
				Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
				DepthStencilFormat depthStencilFmt,
				vk::ImageLayout finalLayout );

	RenderPass(const RenderPass& other) = default;
	~RenderPass() = default;

	RenderPass& 			operator=(const RenderPass& other) = default;

	bool					operator==(const RenderPass& other) const noexcept;
	bool					operator!=(const RenderPass& other) const noexcept;

	vk::RenderPass			getRenderPass() const;

private:
	vk::RenderPass			m_renderPass;

	static vk::RenderPass	createRenderPass(	const Vulkan& vulkan, 
												Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
												DepthStencilFormat depthStencilFmt,
												vk::ImageLayout finalLayout );

};

}