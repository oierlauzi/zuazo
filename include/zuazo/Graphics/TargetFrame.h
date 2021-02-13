#pragma once

#include "Vulkan.h"
#include "Frame.h"
#include "CommandBuffer.h"
#include "DepthStencil.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "../Utils/BufferView.h"

#include <vector>

namespace Zuazo::Graphics {

class TargetFrame 
	: public Frame 
{
public:
	TargetFrame(const Vulkan& vulkan,
				std::shared_ptr<const Descriptor> desc,
				std::shared_ptr<const InputColorTransfer> colorTransfer,
				std::shared_ptr<const Buffer> colorTransferBuffer,
				Utils::BufferView<const Image::Plane> planes,
				std::shared_ptr<const DepthStencil> depthStencil,
				RenderPass renderPass );
	TargetFrame(const TargetFrame& other) = delete;
	TargetFrame(TargetFrame&& other) noexcept = default;
	virtual ~TargetFrame(); 

	TargetFrame& 								operator=(const TargetFrame& other) = delete;

	const Framebuffer&							getFramebuffer() const noexcept;
	void										beginRenderPass(	vk::CommandBuffer cmd, 
																	vk::Rect2D renderArea,
																	Utils::BufferView<const vk::ClearValue> clearValues,
																	vk::SubpassContents contents ) const noexcept;
	void										endRenderPass(vk::CommandBuffer cmd) const noexcept;
	void										draw(std::shared_ptr<const CommandBuffer> cmd);

private:
	std::shared_ptr<const DepthStencil> 		m_depthStencil;
	RenderPass 									m_renderPass;
	Framebuffer									m_framebuffer;
	vk::UniqueFence								m_renderComplete;

	std::shared_ptr<const CommandBuffer>		m_commandBuffer;


	static Framebuffer							createFramebuffer(	const Vulkan& vulkan,
																	const Image& image,
																	const DepthStencil* depthStencil,
																	RenderPass renderPass );

};

}