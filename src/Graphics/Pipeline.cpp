#include <Graphics/Pipeline.h>

namespace Zuazo::Graphics {

Pipeline::Pipeline(	const Vulkan& vulkan,
					vk::RenderPass renderPass,
					vk::Extent2D viewportSize,
					vk::PrimitiveTopology topology,
					const Utils::BufferView<vk::VertexInputBindingDescription>& vertexBindings,
					const Utils::BufferView<vk::VertexInputAttributeDescription>& vertexAttributes,
					vk::ShaderModule vertexShader,
					vk::ShaderModule fragmentShader,
					bool enableBlending,
					const Utils::BufferView<vk::DescriptorSetLayout>& uniforms )
	: m_pipelineLayout(createPipelineLayout(vulkan, uniforms))
	, m_pipeline(createPipeline(vulkan,
		renderPass,
		viewportSize,
		topology,
		vertexBindings,
		vertexAttributes,
		vertexShader,
		fragmentShader,
		enableBlending,
		*m_pipelineLayout
	))
{
}

vk::UniquePipelineLayout Pipeline::createPipelineLayout(const Vulkan& vulkan,
														const Utils::BufferView<vk::DescriptorSetLayout>& uniforms )
{
	const vk::PipelineLayoutCreateInfo createInfo(
		{},
		uniforms.size(), uniforms.data(),
		0,
		nullptr
	);

	return vulkan.getDevice().createPipelineLayoutUnique(
		createInfo, nullptr, 
		vulkan.getDispatcher()
	);
}

vk::UniquePipeline Pipeline::createPipeline(const Vulkan& vulkan,
											vk::RenderPass renderPass,
											vk::Extent2D viewportSize,
											vk::PrimitiveTopology topology,
											const Utils::BufferView<vk::VertexInputBindingDescription>& vertexBindings,
											const Utils::BufferView<vk::VertexInputAttributeDescription>& vertexAttributes,
											vk::ShaderModule vertexShader,
											vk::ShaderModule fragmentShader,
											bool enableBlending,
											vk::PipelineLayout layout )
{
	constexpr auto SHADER_ENTRY_POINT = "main";

	const std::array shaderStages = {
		vk::PipelineShaderStageCreateInfo(		
			{},
			vk::ShaderStageFlagBits::eVertex,
			vertexShader,
			SHADER_ENTRY_POINT ),
		vk::PipelineShaderStageCreateInfo(		
			{},
			vk::ShaderStageFlagBits::eFragment,
			fragmentShader,
			SHADER_ENTRY_POINT ),
	};

	const vk::PipelineVertexInputStateCreateInfo vertexInput(
		{},
		vertexBindings.size(), vertexBindings.data(),
		vertexAttributes.size(), vertexAttributes.data()
	);

	const vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
		{},
		topology
	);

	const std::array viewports = {
		vk::Viewport(
			0.0f, 0.0f,
			static_cast<float>(viewportSize.width), 
			static_cast<float>(viewportSize.height),
			0.0f, 1.0f
		),
	};

	const std::array scissors = {
		vk::Rect2D(
			{ 0, 0 },
			viewportSize
		),
	};

	const vk::PipelineViewportStateCreateInfo viewport(
		{},
		viewports.size(), viewports.data(),
		scissors.size(), scissors.data()
	);

	const vk::PipelineRasterizationStateCreateInfo rasterizer(
		{},
		false, false,
		vk::PolygonMode::eFill,
		vk:: CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise,
		false, 0.0f, 0.0f, 0.0f,
		1.0f
	);

	const vk::PipelineMultisampleStateCreateInfo multisample(
		{},
		vk::SampleCountFlagBits::e1,
		false, 1.0f,
		nullptr,
		false, false
	);

	const vk::PipelineDepthStencilStateCreateInfo depthStencil(
		{},
		false, false, vk::CompareOp::eNever, false,
		false, {}, {},
		0.0f, 0.0f
	);

	const std::array colorBlendAttachments = {
		vk::PipelineColorBlendAttachmentState(
			enableBlending,
			vk::BlendFactor::eSrcAlpha,
			vk::BlendFactor::eOneMinusSrcAlpha,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eSrcAlpha,
			vk::BlendFactor::eOneMinusSrcAlpha,
			vk::BlendOp::eAdd
		)
	};

	const vk::PipelineColorBlendStateCreateInfo colorBlend(
		{},
		false,
		vk::LogicOp::eCopy,
		colorBlendAttachments.size(), colorBlendAttachments.data()
	);

	const std::array dynamicStates = {
		vk::DynamicState::eViewport
	};

	const vk::PipelineDynamicStateCreateInfo dynamicState(
		{},
		dynamicStates.size(), dynamicStates.data()
	);

	const vk::GraphicsPipelineCreateInfo createInfo(
		{},
		shaderStages.size(), shaderStages.data(),
		&vertexInput,
		&inputAssembly,
		nullptr,
		&viewport,
		&rasterizer,
		&multisample,
		&depthStencil,
		&colorBlend,
		&dynamicState,
		layout,
		renderPass, 0,
		nullptr, -1 //TODO
	);

	return vulkan.getDevice().createGraphicsPipelineUnique(
		nullptr, createInfo, nullptr,
		vulkan.getDispatcher()
	);
}
}