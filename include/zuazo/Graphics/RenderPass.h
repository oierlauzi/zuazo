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
				vk::Format intermediaryFmt,
				vk::ImageLayout finalLayout );

	RenderPass(const RenderPass& other) = default;
	~RenderPass() = default;

	RenderPass& 									operator=(const RenderPass& other) = default;

	bool											operator==(const RenderPass& other) const noexcept;
	bool											operator!=(const RenderPass& other) const noexcept;

	vk::RenderPass						get() const noexcept;

	void								finalize(const Vulkan& vulkan, vk::CommandBuffer cmd) const noexcept;

	static std::vector<vk::ClearValue>	getClearValues(DepthStencilFormat depthStencilFmt);
	static vk::DescriptorSetLayout		getFinalizationDescriptorSetLayout(const Vulkan& vulkan);
	static vk::PipelineLayout			getFinalizationPipelineLayout(const Vulkan& vulkan);

private:
	vk::RenderPass						m_renderPass;
	vk::Pipeline						m_finalizationPipeline;

	static vk::RenderPass				createRenderPass(	const Vulkan& vulkan, 
															Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
															vk::Format intermediaryFmt,
															vk::Format depthStencilFmt,
															vk::ImageLayout finalLayout );

	static vk::Pipeline 				createFinalizationPipeline(	const Graphics::Vulkan& vulkan,
																	vk::RenderPass renderPass );

};

}