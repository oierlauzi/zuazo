#pragma once

#include "Vulkan.h"
#include "Frame.h"

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
				std::shared_ptr<const vk::UniqueRenderPass> renderPass );
	TargetFrame(const TargetFrame& other) = delete;
	TargetFrame(TargetFrame&& other) = default;
	virtual ~TargetFrame(); 

	TargetFrame& 								operator=(const TargetFrame& other) = delete;

private:
	std::shared_ptr<const vk::UniqueRenderPass> m_renderPass;

	vk::UniqueFramebuffer						m_frameBuffer;

	vk::UniqueFence								m_renderComplete;

	static vk::UniqueFramebuffer				createFrameBuffer(	const Graphics::Vulkan& vulkan,
																	Utils::BufferView<const PlaneDescriptor> planes,
																	const std::shared_ptr<const vk::UniqueRenderPass>& renderPass,
																	const std::vector<vk::UniqueImageView>& imageViews );

};

}