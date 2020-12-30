#pragma once

#include "Vulkan.h"
#include "Frame.h"
#include "CommandBuffer.h"
#include "DepthStencil.h"
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
				std::shared_ptr<const Buffer> colorTransfer,
				Utils::BufferView<const Image::PlaneDescriptor> planes,
				std::shared_ptr<const DepthStencil> depthStencil,
				RenderPass renderPass );
	TargetFrame(const TargetFrame& other) = delete;
	TargetFrame(TargetFrame&& other) noexcept = default;
	virtual ~TargetFrame(); 

	TargetFrame& 								operator=(const TargetFrame& other) = delete;

	vk::Framebuffer								getFramebuffer() const noexcept;
	void										beginRenderPass(	vk::CommandBuffer cmd, 
																	vk::Rect2D renderArea,
																	Utils::BufferView<const vk::ClearValue> clearValues,
																	vk::SubpassContents contents ) const noexcept;
	void										endRenderPass(vk::CommandBuffer cmd) const noexcept;
	void										draw(std::shared_ptr<const CommandBuffer> cmd);

private:
	std::shared_ptr<const DepthStencil> 		m_depthStencil;
	RenderPass 									m_renderPass;
	vk::UniqueFramebuffer						m_framebuffer;
	vk::UniqueFence								m_renderComplete;

	std::shared_ptr<const CommandBuffer>		m_commandBuffer;


	static vk::UniqueFramebuffer				createFramebuffer(	const Graphics::Vulkan& vulkan,
																	Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
																	const std::shared_ptr<const DepthStencil>& depthStencil,
																	vk::RenderPass renderPass,
																	const Image& image );

};

}