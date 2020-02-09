#pragma once

#include "Vulkan.h"
#include "../Utils/BufferView.h"

namespace Zuazo::Graphics {

class Pipeline {
public:
	Pipeline(	const Vulkan& vulkan,
				vk::RenderPass renderPass,
				vk::Extent2D viewportSize,
				vk::PrimitiveTopology topology,
				const Utils::BufferView<vk::VertexInputBindingDescription>& vertexBindings,
				const Utils::BufferView<vk::VertexInputAttributeDescription>& vertexAttributes,
				vk::ShaderModule vertexShader,
				vk::ShaderModule fragmentShader,
				bool enableBlending,
				const Utils::BufferView<vk::DescriptorSetLayout>& uniforms );	
	Pipeline(const Pipeline& other) = delete;
	Pipeline(Pipeline&& opther) = default;
	~Pipeline() = default;

	Pipeline& operator=(const Pipeline& other) = delete;
	Pipeline& operator=(Pipeline&& other) = default;

private:
	vk::UniquePipelineLayout 	m_pipelineLayout;
	vk::UniquePipeline			m_pipeline;

	static vk::UniquePipelineLayout createPipelineLayout(	const Vulkan& vulkan,
															const Utils::BufferView<vk::DescriptorSetLayout>& uniforms );

	static vk::UniquePipeline createPipeline(	const Vulkan& vulkan,
												vk::RenderPass renderPass,
												vk::Extent2D viewportSize,
												vk::PrimitiveTopology topology,
												const Utils::BufferView<vk::VertexInputBindingDescription>& vertexBindings,
												const Utils::BufferView<vk::VertexInputAttributeDescription>& vertexAttributes,
												vk::ShaderModule vertexShader,
												vk::ShaderModule fragmentShader,
												bool enableBlending,
												vk::PipelineLayout layout );	
};

}