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

vk::PipelineLayout Pipeline::getPipelineLayout() const{
	return *m_pipelineLayout;
}

vk::Pipeline Pipeline::getPipeline() const{
	return *m_pipeline;
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
			//Cf' = Ai*Ci + (1.0-Ai)*Cf; Typical color mixing equation
			vk::BlendFactor::eSrcAlpha,
			vk::BlendFactor::eOneMinusSrcAlpha,
			vk::BlendOp::eAdd,
			//Af' = Ai + (1.0-Ai)*Af = Ai + Af - Ai*Af; So that Af' is always greater than Af and Ai
			//https://www.wolframalpha.com/input/?i=plot+%7C+x+%2B+y+-+x+y+%7C+x+%3D+0+to+1+y+%3D+0+to+1
			vk::BlendFactor::eOne,
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

	const vk::PipelineDynamicStateCreateInfo dynamicState(
		{},
		0, nullptr
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