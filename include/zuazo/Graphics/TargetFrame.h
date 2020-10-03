#pragma once

#include "Vulkan.h"
#include "Frame.h"
#include "CommandBuffer.h"
#include "DepthStencil.h"
#include "../Utils/BufferView.h"

#include <vector>

namespace Zuazo::Graphics {

class TargetFrame 
	: public Frame 
{
public:
	TargetFrame(const Vulkan& vulkan,
				std::shared_ptr<const Descriptor> desc,
				std::shared_ptr<const Buffer> colorTransfer,
				Utils::BufferView<const PlaneDescriptor> planes,
				std::shared_ptr<const vk::UniqueRenderPass> renderPass,
				std::shared_ptr<const DepthStencil> depthStencil );
	TargetFrame(const TargetFrame& other) = delete;
	TargetFrame(TargetFrame&& other) = default;
	virtual ~TargetFrame(); 

	TargetFrame& 								operator=(const TargetFrame& other) = delete;

	vk::Framebuffer								getFramebuffer() const;
	void										beginRenderPass(	vk::CommandBuffer cmd, 
																	vk::Rect2D renderArea,
																	Utils::BufferView<const vk::ClearValue> clearValues,
																	vk::SubpassContents contents ) const;
	void										endRenderPass(vk::CommandBuffer cmd) const;
	void										draw(std::shared_ptr<const CommandBuffer> cmd);

private:
	std::shared_ptr<const vk::UniqueRenderPass> m_renderPass;
	std::shared_ptr<const DepthStencil> 		m_depthStencil;
	vk::UniqueFramebuffer						m_framebuffer;
	vk::UniqueFence								m_renderComplete;

	std::shared_ptr<const CommandBuffer>		m_commandBuffer;


	static vk::UniqueFramebuffer				createFramebuffer(	const Graphics::Vulkan& vulkan,
																	Utils::BufferView<const PlaneDescriptor> planes,
																	const std::shared_ptr<const vk::UniqueRenderPass>& renderPass,
																	const std::shared_ptr<const DepthStencil>& depthStencil,
																	const std::vector<vk::UniqueImageView>& imageViews );

};

}