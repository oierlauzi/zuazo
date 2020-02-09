#pragma once

#include "Vulkan.h"

#include "../Utils/BufferView.h"

namespace Zuazo::Graphics {

class RenderPass {
public:
	RenderPass(	const Vulkan& vulkan,
				const Utils::BufferView<vk::AttachmentDescription>& attachments,
				const Utils::BufferView<vk::SubpassDescription>& subpasses);	
	RenderPass(const RenderPass& other) = delete;
	RenderPass(RenderPass&& opther) = default;
	~RenderPass() = default;

	RenderPass& operator=(const RenderPass& other) = delete;
	RenderPass& operator=(RenderPass&& other) = default;

	vk::RenderPass				getRenderPass();
private:
	vk::UniqueRenderPass		m_renderPass;

	static vk::UniqueRenderPass createRenderPass(	const Vulkan& vulkan,
													const Utils::BufferView<vk::AttachmentDescription>& attachments,
													const Utils::BufferView<vk::SubpassDescription>& subpasses );
};

}