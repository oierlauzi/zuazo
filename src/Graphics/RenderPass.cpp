#include <zuazo/Graphics/RenderPass.h>

#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Utils/StaticId.h>

#include <algorithm>

namespace Zuazo::Graphics {

RenderPass::RenderPass(	const Vulkan& vulkan, 
						Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
						DepthStencilFormat depthStencilFmt,
						vk::Format intermediaryFmt,
						vk::ImageLayout finalLayout )
	: m_renderPass(createRenderPass(vulkan, planeDescriptors, toVulkan(depthStencilFmt), intermediaryFmt, finalLayout))
	, m_finalizationPipeline((intermediaryFmt != vk::Format::eUndefined) ? createFinalizationPipeline(vulkan, m_renderPass) : vk::Pipeline())
{
}

bool RenderPass::operator==(const RenderPass& other) const noexcept {
	return m_renderPass == other.m_renderPass;
}

bool RenderPass::operator!=(const RenderPass& other) const noexcept {
	return !operator==(other);
}



vk::RenderPass RenderPass::get() const noexcept {
	return m_renderPass;
}



void RenderPass::finalize(const Vulkan& vulkan, vk::CommandBuffer cmd) const noexcept {
	if(m_finalizationPipeline) {
		vulkan.nextSubpass(cmd, vk::SubpassContents::eInline);
		vulkan.bindPipeline(cmd, vk::PipelineBindPoint::eGraphics, m_finalizationPipeline);
		vulkan.draw(cmd, 3, 1, 0, 0); //Draws a fullscreen tri
	}
}



std::vector<vk::ClearValue>	RenderPass::getClearValues(DepthStencilFormat depthStencilFmt) {
	std::vector<vk::ClearValue> result; 
	const bool hasDepthStencil = DepthStencilFormat::NONE < depthStencilFmt && depthStencilFmt < DepthStencilFormat::COUNT;
	result.reserve(hasDepthStencil + 1);

	//First attachment is the depth/stencil attachment if present
	if(hasDepthStencil) {
		result.emplace_back(vk::ClearDepthStencilValue(1.0f, 0x00)); //Clear to the far plane
	}
	
	//The next attachment is the intermediary or result attachment
	result.emplace_back(vk::ClearValue()); //Default initialized to 0.0f-s

	return result;
}

vk::DescriptorSetLayout	RenderPass::getFinalizationDescriptorSetLayout(const Vulkan& vulkan) {
	static const Utils::StaticId id;

	auto result = vulkan.createDescriptorSetLayout(id);

	if(!result) {
		enum {
			FINALIZATION_BINDING_OUTPUT_COLOR_TRANSFER,
			FINALIZATION_BINDING_INPUT_ATTACHMENT,

			FINALIZATION_BINDING_COUNT
		};

		//Create the bindings
		const std::array bindings = {
			vk::DescriptorSetLayoutBinding(	//UBO binding
				FINALIZATION_BINDING_OUTPUT_COLOR_TRANSFER,		//Binding
				vk::DescriptorType::eUniformBuffer,				//Type
				1,												//Count
				vk::ShaderStageFlagBits::eFragment,				//Shader stage
				nullptr											//Immutable samplers
			), 
			vk::DescriptorSetLayoutBinding(	//Input attachment binding
				FINALIZATION_BINDING_INPUT_ATTACHMENT,			//Binding
				vk::DescriptorType::eInputAttachment,			//Type
				1,												//Count
				vk::ShaderStageFlagBits::eFragment,				//Shader stage
				nullptr											//Immutable samplers
			), 
		};

		const vk::DescriptorSetLayoutCreateInfo createInfo(
			{},
			bindings.size(), bindings.data()
		);

		result = vulkan.createDescriptorSetLayout(id, createInfo);
	}

	assert(result);
	return result;
}

vk::PipelineLayout RenderPass::getFinalizationPipelineLayout(const Vulkan& vulkan) {
	static const Utils::StaticId id;

	auto result = vulkan.createPipelineLayout(id);

	if(!result) {
		const std::array layouts {
			getFinalizationDescriptorSetLayout(vulkan)
		};

		const vk::PipelineLayoutCreateInfo createInfo(
			{},													//Flags
			layouts.size(), layouts.data(),						//Descriptor set layouts
			0, nullptr											//Push constants
		);

		result = vulkan.createPipelineLayout(id, createInfo);
	}

	assert(result);
	return result;
}


vk::RenderPass RenderPass::createRenderPass(const Vulkan& vulkan, 
											Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
											vk::Format depthStencilFmt,
											vk::Format intermediaryFmt,
											vk::ImageLayout finalLayout )
{
	//Get an id for this configuration
	static std::unordered_map<uint64_t, const Utils::StaticId> ids; //TODO make thread-safe

	uint64_t index = 0;

	constexpr uint64_t COLOR_FORMAT_MULT = 0x100; //4*8 = 32bits
	for(const auto& plane : planeDescriptors) {
		assert(plane.swizzle == vk::ComponentMapping());

		index *= COLOR_FORMAT_MULT;
		index += static_cast<uint64_t>(plane.format) % COLOR_FORMAT_MULT; //TODO this does not work well with extensions, too big numbers.
	}
	for(size_t i = planeDescriptors.size(); i < 4; ++i) {
		index *= COLOR_FORMAT_MULT;
	}

	constexpr uint64_t DEPTH_STENCIL_MULT = 0x100; //8 bits
	index *= DEPTH_STENCIL_MULT;
	index += static_cast<uint64_t>(depthStencilFmt) % DEPTH_STENCIL_MULT;

	constexpr uint64_t INTERMEDIARY_MULT = 0x100; //8 bits
	index *= INTERMEDIARY_MULT;
	index += static_cast<uint64_t>(intermediaryFmt) % INTERMEDIARY_MULT; //TODO this does not work well with extensions, too big numbers.

	constexpr uint64_t FINAL_LAYOUT_MULT = 0x100; //8bits
	index *= FINAL_LAYOUT_MULT;
	index += static_cast<uint64_t>(finalLayout) % FINAL_LAYOUT_MULT; //TODO this does not work well with extensions, too big numbers.

	const size_t id = ids[index];

	//Check if a renderpass with this id exists
	vk::RenderPass result = vulkan.createRenderPass(id);
	if(!result) {
		//Get the element count for the arrays
		const size_t colorAttachmentCount = planeDescriptors.size();
		const size_t intermediaryAttachmentCount = (intermediaryFmt != vk::Format::eUndefined) ? 1 : 0;
		const size_t depthStencilAttachmentCount = (depthStencilFmt != vk::Format::eUndefined) ? 1 : 0;
		const size_t attachmentCount = colorAttachmentCount + intermediaryAttachmentCount + depthStencilAttachmentCount;

		/*
		 * The attachment layout will be the following:
		 * 0. Depth/Stencil if present
		 * 1. Intermediary if present
		 * 3. Result attachments
		 * 
		 * This way, by specifying clear values for the first 2 if depth
		 * is present or 1 if not, all needed clear values are specified.
		 * Note that if the intermediary target is not present, there should
		 * be only one result attachment, whose role is the same as the 
		 * intermediary one.
		 */
		assert(colorAttachmentCount == 1 || intermediaryAttachmentCount == 1);

		//Create the attachments descriptors
		std::vector<vk::AttachmentDescription> attachments;
		attachments.reserve(attachmentCount);

		//Depth/Stencil attachment if present
		if(depthStencilAttachmentCount) {
			attachments.emplace_back(
				vk::AttachmentDescriptionFlags(),				//Flags
				depthStencilFmt,								//Attachemnt format
				vk::SampleCountFlagBits::e1,					//Sample count
				vk::AttachmentLoadOp::eClear,					//Depth attachment load operation
				vk::AttachmentStoreOp::eDontCare,				//Depth attachemnt store operation
				vk::AttachmentLoadOp::eClear,					//Stencil attachment load operation
				vk::AttachmentStoreOp::eDontCare,				//Stencil attachment store operation
				vk::ImageLayout::eUndefined,					//Initial layout
				vk::ImageLayout::eDepthStencilAttachmentOptimal	//Final layout
			);
		}

		//Intermediary attachment if present
		if(intermediaryAttachmentCount) {
			attachments.emplace_back(
				vk::AttachmentDescriptionFlags(),				//Flags
				intermediaryFmt,								//Attachemnt format
				vk::SampleCountFlagBits::e1,					//Sample count
				vk::AttachmentLoadOp::eClear,					//Color attachment load operation
				vk::AttachmentStoreOp::eDontCare,				//Color attachment store operation
				vk::AttachmentLoadOp::eDontCare,				//Stencil attachment load operation
				vk::AttachmentStoreOp::eDontCare,				//Stencil attachment store operation
				vk::ImageLayout::eUndefined,					//Initial layout
				vk::ImageLayout::eColorAttachmentOptimal		//Final layout
			);
		}

		//Result attachment
		for(const auto& plane : planeDescriptors) {
			//Only clear if it won't get overriden by the 2nd subpass
			const auto loadOp = 
				intermediaryAttachmentCount ?
				vk::AttachmentLoadOp::eDontCare :
				vk::AttachmentLoadOp::eClear ;

			attachments.emplace_back(
				vk::AttachmentDescriptionFlags(),				//Flags
				plane.format,									//Attachemnt format
				vk::SampleCountFlagBits::e1,					//Sample count
				loadOp,											//Color attachment load operation
				vk::AttachmentStoreOp::eStore,					//Color attachemnt store operation
				vk::AttachmentLoadOp::eDontCare,				//Stencil attachment load operation
				vk::AttachmentStoreOp::eDontCare,				//Stencil attachment store operation
				vk::ImageLayout::eUndefined,					//Initial layout
				finalLayout										//Final layout
			);
		}
		assert(attachments.size() == attachmentCount);

		//Create the color attachment references for the result
		std::vector<vk::AttachmentReference> colorAttachmentReferences;
		colorAttachmentReferences.reserve(OutputColorTransfer::getAttachmentCount()); //Worst case scenario
		for(size_t i = 0; i < colorAttachmentCount; ++i) {
			colorAttachmentReferences.emplace_back(
				depthStencilAttachmentCount + intermediaryAttachmentCount + i,	//Attachments index
				vk::ImageLayout::eColorAttachmentOptimal 						//Attachemnt layout
			);
		}
		assert(colorAttachmentReferences.size() == colorAttachmentCount);

		//Create the color attachment references for the depth/stencil buffer
		constexpr vk::AttachmentReference depthStencilAttachmentReference(
			0, 													//Attachments index
			vk::ImageLayout::eDepthStencilAttachmentOptimal 	//Attachemnt layout
		);

		//Create the subpass descriptors and dependencies
		std::vector<vk::SubpassDescription> subpassDescriptors;
		std::vector<vk::SubpassDependency> subpassDependencies;
		if(intermediaryAttachmentCount) {
			//There will be 2 subpasses

			//Color attachment needs to be padded, as there will be 4 color outputs from
			//the finalization shader:
			for(size_t i = colorAttachmentCount; i < OutputColorTransfer::getAttachmentCount(); ++i) {
				colorAttachmentReferences.emplace_back(
					VK_ATTACHMENT_UNUSED, 							//Attachments index
					vk::ImageLayout::eColorAttachmentOptimal 		//Attachemnt layout
				);	
			}
			assert(colorAttachmentReferences.size() == OutputColorTransfer::getAttachmentCount());

			//Create the color attachment references for intermediary color attachment
			const std::array intermediaryColorAttachmentReferences0 = {
				vk::AttachmentReference(
					depthStencilAttachmentCount,					//Attachments index
					vk::ImageLayout::eColorAttachmentOptimal 		//Attachemnt layout
				)
			};
			const std::array intermediaryColorAttachmentReferences1 = {
				vk::AttachmentReference(
					depthStencilAttachmentCount, 					//Attachments index
					vk::ImageLayout::eShaderReadOnlyOptimal 		//Attachemnt layout
				)
			};
			static_assert(intermediaryColorAttachmentReferences0.size() == 1, "Intermediary attachment count must be 1");
			static_assert(intermediaryColorAttachmentReferences1.size() == 1, "Intermediary attachment count must be 1");
			assert(intermediaryColorAttachmentReferences0.front().attachment == intermediaryColorAttachmentReferences1.front().attachment);

			subpassDescriptors = {
				vk::SubpassDescription(
					{},																								//Flags
					vk::PipelineBindPoint::eGraphics,																//Pipeline bind point
					0, nullptr,																						//Input attachments
					intermediaryColorAttachmentReferences0.size(), intermediaryColorAttachmentReferences0.data(),	//Color attachments
					nullptr,																						//Resolve attachment
					depthStencilAttachmentCount ? &depthStencilAttachmentReference : nullptr, 						//Depth-stencil attachment
					0, nullptr																						//Preserve attachments
				),			
				vk::SubpassDescription(
					{},																								//Flags
					vk::PipelineBindPoint::eGraphics,																//Pipeline bind point
					intermediaryColorAttachmentReferences1.size(), intermediaryColorAttachmentReferences1.data(),	//Input attachments
					colorAttachmentReferences.size(), colorAttachmentReferences.data(), 							//Color attachments
					nullptr,																						//Resolve attachment
					nullptr, 																						//Depth-stencil attachment
					0, nullptr																						//Preserve attachments
				)
			};

			subpassDependencies = {
				vk::SubpassDependency(
					VK_SUBPASS_EXTERNAL,																	//Src subpass
					0,																						//Dst subpass
					vk::PipelineStageFlagBits::eBottomOfPipe,												//Src pipeline stages
					vk::PipelineStageFlagBits::eColorAttachmentOutput,										//Dst pipeline stages
					vk::AccessFlagBits::eMemoryRead,														//Src access mask
					vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,	//Dst access mask
					vk::DependencyFlagBits::eByRegion														//Dependency flags
				),
				vk::SubpassDependency(
					0,																						//Src subpass
					1,																						//Dst subpass
					vk::PipelineStageFlagBits::eColorAttachmentOutput,										//Src pipeline stages
					vk::PipelineStageFlagBits::eFragmentShader,												//Dst pipeline stages
					vk::AccessFlagBits::eColorAttachmentWrite,												//Src access mask
					vk::AccessFlagBits::eShaderRead,														//Dst access mask
					vk::DependencyFlagBits::eByRegion														//Dependency flags
				),
				vk::SubpassDependency(
					1,																						//Src subpass
					VK_SUBPASS_EXTERNAL,																	//Dst subpass
					vk::PipelineStageFlagBits::eColorAttachmentOutput,										//Src pipeline stages
					vk::PipelineStageFlagBits::eBottomOfPipe,												//Dst pipeline stages
					vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,	//Src access mask
					vk::AccessFlagBits::eMemoryRead,														//Dst access mask
					vk::DependencyFlagBits::eByRegion														//Dependency flags
				)
			};
		} else {
			//Result will be processed in a single subpass

			//There should be a single color attachment, as we're directly writting the output of the FS
			assert(colorAttachmentReferences.size() == 1);

			subpassDescriptors = {
				vk::SubpassDescription(
					{},																						//Flags
					vk::PipelineBindPoint::eGraphics,														//Pipeline bind point
					0, nullptr,																				//Input attachments
					colorAttachmentReferences.size(), colorAttachmentReferences.data(), 					//Color attachments
					nullptr,																				//Resolve attachment
					depthStencilAttachmentCount ? &depthStencilAttachmentReference : nullptr, 				//Depth-stencil attachment
					0, nullptr																				//Preserve attachments
				)
			};

			subpassDependencies = {
				vk::SubpassDependency(
					VK_SUBPASS_EXTERNAL,																	//Src subpass
					0,																						//Dst subpass
					vk::PipelineStageFlagBits::eBottomOfPipe,												//Src pipeline stages
					vk::PipelineStageFlagBits::eColorAttachmentOutput,										//Dst pipeline stages
					vk::AccessFlagBits::eMemoryRead,														//Src access mask
					vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,	//Dst access mask
					vk::DependencyFlagBits::eByRegion														//Dependency flags
				),
				vk::SubpassDependency(
					0,																						//Src subpass
					VK_SUBPASS_EXTERNAL,																	//Dst subpass
					vk::PipelineStageFlagBits::eColorAttachmentOutput,										//Src pipeline stages
					vk::PipelineStageFlagBits::eBottomOfPipe,												//Dst pipeline stages
					vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,	//Src access mask
					vk::AccessFlagBits::eMemoryRead,														//Dst access mask
					vk::DependencyFlagBits::eByRegion														//Dependency flags
				)
			};
		}

		//Create the renderpass
		const vk::RenderPassCreateInfo createInfo(
			{},														//Flags
			attachments.size(), attachments.data(),					//Attachments
			subpassDescriptors.size(), subpassDescriptors.data(), 	//Subpasses
			subpassDependencies.size(), subpassDependencies.data()	//Subpass dependencies
		);

		result = vulkan.createRenderPass(id, createInfo);
	}

	assert(result);
	return result;
}

vk::Pipeline RenderPass::createFinalizationPipeline(const Graphics::Vulkan& vulkan,
													vk::RenderPass renderPass )
{
	static std::unordered_map<vk::RenderPass, const Utils::StaticId> ids; //TODO make thread safe
	const auto& id = ids[renderPass];

	vk::Pipeline result = vulkan.createGraphicsPipeline(id);

	if(!result) {
		static //So that its ptr can be used as an identifier
		#include <renderpass_finalization_vert.h>
		const size_t vertId = reinterpret_cast<uintptr_t>(renderpass_finalization_vert);
		static
		#include <renderpass_finalization_frag.h>
		const size_t fragId = reinterpret_cast<uintptr_t>(renderpass_finalization_frag);

		//Try to retrive modules from cache
		auto vertexShader = vulkan.createShaderModule(vertId);
		if(!vertexShader) {
			//Module isn't in cache. Create it
			vertexShader = vulkan.createShaderModule(vertId, renderpass_finalization_vert);
		}

		auto fragmentShader = vulkan.createShaderModule(fragId);
		if(!fragmentShader) {
			//Module isn't in cache. Create it
			fragmentShader = vulkan.createShaderModule(fragId, renderpass_finalization_frag);
		}

		assert(vertexShader);
		assert(fragmentShader);

		constexpr auto SHADER_ENTRY_POINT = "main";
		const std::array shaderStages = {
			vk::PipelineShaderStageCreateInfo(		
				{},												//Flags
				vk::ShaderStageFlagBits::eVertex,				//Shader type
				vertexShader,									//Shader handle
				SHADER_ENTRY_POINT ),							//Shader entry point
			vk::PipelineShaderStageCreateInfo(		
				{},												//Flags
				vk::ShaderStageFlagBits::eFragment,				//Shader type
				fragmentShader,									//Shader handle
				SHADER_ENTRY_POINT ),							//Shader entry point
		};

		constexpr vk::PipelineVertexInputStateCreateInfo vertexInput(
			{},
			0, nullptr,											//Vertex bindings
			0, nullptr											//Vertex attributes
		);

		constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
			{},													//Flags
			vk::PrimitiveTopology::eTriangleList,				//Topology
			false												//Restart enable
		);

		constexpr vk::PipelineViewportStateCreateInfo viewport(
			{},													//Flags
			1, nullptr,											//Viewports (dynamic)
			1, nullptr											//Scissors (dynamic)
		);

		constexpr vk::PipelineRasterizationStateCreateInfo rasterizer(
			{},													//Flags
			false, 												//Depth clamp enabled
			false,												//Rasterizer discard enable
			vk::PolygonMode::eFill,								//Polygon mode
			vk::CullModeFlagBits::eNone, 						//Cull faces
			vk::FrontFace::eClockwise,							//Front face direction
			false, 0.0f, 0.0f, 0.0f,							//Depth bias
			1.0f												//Line width
		);

		constexpr vk::PipelineMultisampleStateCreateInfo multisample(
			{},													//Flags
			vk::SampleCountFlagBits::e1,						//Sample count
			false, 1.0f,										//Sample shading enable, min sample shading
			nullptr,											//Sample mask
			false, false										//Alpha to coverage, alpha to 1 enable
		);

		constexpr vk::PipelineDepthStencilStateCreateInfo depthStencil(
			{},													//Flags
			false, false, 										//Depth test enable, write
			vk::CompareOp::eAlways,								//Depth compare op
			false,												//Depth bounds test
			false, 												//Stencil enabled
			{}, {},												//Stencil operation state front, back
			0.0f, 0.0f											//min, max depth bounds
		);

		const std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments(
			OutputColorTransfer::getAttachmentCount(),
			Graphics::toVulkan(BlendingMode::WRITE)
		);

		const vk::PipelineColorBlendStateCreateInfo colorBlend(
			{},													//Flags
			false,												//Enable logic operation
			vk::LogicOp::eCopy,									//Logic operation
			colorBlendAttachments.size(), colorBlendAttachments.data() //Blend attachments
		);

		constexpr std::array dynamicStates = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		const vk::PipelineDynamicStateCreateInfo dynamicState(
			{},													//Flags
			dynamicStates.size(), dynamicStates.data()			//Dynamic states
		);

		const auto pipelineLayout = getFinalizationPipelineLayout(vulkan);

		static const Utils::StaticId pipelineId;
		const vk::GraphicsPipelineCreateInfo createInfo(
			{},													//Flags
			shaderStages.size(), shaderStages.data(),			//Shader stages
			&vertexInput,										//Vertex input
			&inputAssembly,										//Vertex assembly
			nullptr,											//Tesselation
			&viewport,											//Viewports
			&rasterizer,										//Rasterizer
			&multisample,										//Multisampling
			&depthStencil,										//Depth / Stencil tests
			&colorBlend,										//Color blending
			&dynamicState,										//Dynamic states
			pipelineLayout,										//Pipeline layout
			renderPass, 1,										//Renderpasses
			nullptr, static_cast<uint32_t>(pipelineId)			//Inherit
		);

		result = vulkan.createGraphicsPipeline(id, createInfo);
	}

	return result;
}

}