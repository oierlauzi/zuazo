#include <zuazo/Graphics/RenderPass.h>

#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Utils/StaticId.h>

#include <algorithm>
#include <bitset>

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



void RenderPass::finalize(	const Vulkan& vulkan, 
							vk::CommandBuffer cmd, 
							vk::DescriptorSet descriptorSet ) const noexcept 
{
	if(m_finalizationPipeline) {
		vulkan.nextSubpass(cmd, vk::SubpassContents::eInline);

		vulkan.bindDescriptorSets(
			cmd,									//Command buffer
			vk::PipelineBindPoint::eGraphics,		//Pipeline bind point
			getFinalizationPipelineLayout(vulkan),	//Pipeline layout //TODO maybe cache?
			DESCRIPTOR_SET,							//First index
			descriptorSet,							//Descriptor sets
			{}										//Dynamic offsets
		);

		vulkan.bindPipeline(
			cmd, 									//Command buffer
			vk::PipelineBindPoint::eGraphics, 		//Pipeline bind point
			m_finalizationPipeline					//Pipeline
		);

		//Draw a fullscreen triangle
		vulkan.draw(cmd, 3, 1, 0, 0);
	}
}

vk::Format RenderPass::getIntermediateFormat(	const Vulkan& vulkan, 
												Utils::BufferView<const Image::PlaneDescriptor> planes,
												const OutputColorTransfer& colorTransfer )
{
	auto result = vk::Format::eUndefined;
	const auto referenceFormat = planes.front().format;

	if(!colorTransfer.isPassthough() && referenceFormat != vk::Format::eUndefined) {
		enum IntermediateFormatPrecision {
			INTERMEDIATE_FORMAT_PRECISION_16f,
			INTERMEDIATE_FORMAT_PRECISION_32f,
			INTERMEDIATE_FORMAT_PRECISION_64f,

			INTERMEDIATE_FORMAT_PRECISION_COUNT
		};

		constexpr std::array<vk::Format, INTERMEDIATE_FORMAT_PRECISION_COUNT> INTERMEDIATE_FORMATS = {
			vk::Format::eR16G16B16A16Sfloat,
			vk::Format::eR32G32B32A32Sfloat,
			vk::Format::eR64G64B64A64Sfloat
		};

		//Decide which precision will be needed
		int minimumPrecision;
		switch(referenceFormat) {
		//64f used:
		case vk::Format::eR64G64B64A64Sfloat:
		case vk::Format::eR64G64B64Sfloat:
		case vk::Format::eR64G64Sfloat:
		case vk::Format::eR64Sfloat:
			minimumPrecision = INTERMEDIATE_FORMAT_PRECISION_64f;
			break;

		//32f used:
		case vk::Format::eR32G32B32A32Sfloat:
		case vk::Format::eR32G32B32Sfloat:
		case vk::Format::eR32G32Sfloat:
		case vk::Format::eR32Sfloat:
		case vk::Format::eR16G16B16A16Unorm:
		case vk::Format::eR16G16B16Unorm:
		case vk::Format::eR16G16Unorm:
		case vk::Format::eR16Unorm:
		case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16:
		case vk::Format::eR12X4G12X4Unorm2Pack16:
		case vk::Format::eR12X4UnormPack16:
		case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16:
		case vk::Format::eR10X6G10X6Unorm2Pack16:
		case vk::Format::eR10X6UnormPack16:
		case vk::Format::eA2R10G10B10UnormPack32:
		case vk::Format::eA2B10G10R10UnormPack32:
			minimumPrecision = INTERMEDIATE_FORMAT_PRECISION_32f;
			break;

		//16f used:
		default:
			minimumPrecision = INTERMEDIATE_FORMAT_PRECISION_16f;
			break;
		}

		//Obtain the format support
		constexpr vk::FormatFeatureFlags formatFeatures =
			vk::FormatFeatureFlagBits::eColorAttachment |
			vk::FormatFeatureFlagBits::eColorAttachmentBlend ;

		const auto& formatSupport = vulkan.listSupportedFormatsOptimal(formatFeatures);
		assert(std::is_sorted(formatSupport.cbegin(), formatSupport.cend())); //For binary search

		//Decide which format to use based on the minimum precision. If not available,
		//lower the precision (unlikely)
		while(minimumPrecision >= 0) {
			const auto desiredFormat = INTERMEDIATE_FORMATS[minimumPrecision];
			if(std::binary_search(formatSupport.cbegin(), formatSupport.cend(), desiredFormat)) {
				//It is supported
				break;
			}

			--minimumPrecision;
		}

		//Evaluate if the intermediary format was found
		if(minimumPrecision < 0) {
			throw Exception("Unable to find a suitable intermediate format");
		} else {
			result = INTERMEDIATE_FORMATS[minimumPrecision];
		}

		//At this point, we should have succeeded at finding an intermediary format
		assert(result != vk::Format::eUndefined);
	}

	return result;
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



RenderPass::UniformBufferSizes RenderPass::getFinalizationUniformBufferSizes() noexcept {
	static const std::array uniformBufferSizes = {
		std::make_pair(static_cast<uint32_t>(FINALIZATION_DESCRIPTOR_BINDING_OUTPUT_COLOR_TRANSFER), Graphics::OutputColorTransfer::size())
	};

	return uniformBufferSizes;
}

RenderPass::DescriptorPoolSizes RenderPass::getFinalizationDescriptorPoolSizes() noexcept {
	constexpr uint32_t INPUT_ATTACHMENT_COUNT = 1;
	static const std::array descriptorPoolSizes = {
		vk::DescriptorPoolSize(
			vk::DescriptorType::eUniformBuffer,
			getFinalizationUniformBufferSizes().size()
		),
		vk::DescriptorPoolSize(
			vk::DescriptorType::eInputAttachment,
			INPUT_ATTACHMENT_COUNT
		)
	};

	return descriptorPoolSizes;
}

vk::DescriptorSetLayout	RenderPass::getFinalizationDescriptorSetLayout(const Vulkan& vulkan) {
	static const Utils::StaticId id;

	auto result = vulkan.createDescriptorSetLayout(id);

	if(!result) {
		//Create the bindings
		const std::array bindings = {
			vk::DescriptorSetLayoutBinding(	//UBO binding
				FINALIZATION_DESCRIPTOR_BINDING_OUTPUT_COLOR_TRANSFER,	//Binding
				vk::DescriptorType::eUniformBuffer,						//Type
				1,														//Count
				vk::ShaderStageFlagBits::eFragment,						//Shader stage
				nullptr													//Immutable samplers
			), 
			vk::DescriptorSetLayoutBinding(	//Input attachment binding
				FINALIZATION_DESCRIPTOR_BINDING_INPUT_ATTACHMENT,		//Binding
				vk::DescriptorType::eInputAttachment,					//Type
				1,														//Count
				vk::ShaderStageFlagBits::eFragment,						//Shader stage
				nullptr													//Immutable samplers
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

void RenderPass::writeFinalizationDescriptorSet(const Graphics::Vulkan& vulkan,
												vk::DescriptorSet descriptorSet,
												const Image& intermediaryImage )
{
	assert(intermediaryImage.getPlanes().size() == 1);
	const auto imageView = *(intermediaryImage.getPlanes().front().imageView);

	const vk::DescriptorImageInfo descriptorImageInfo(
		nullptr,											//Sampler
		imageView,											//Image view
		vk::ImageLayout::eShaderReadOnlyOptimal				//Image layout
	);

	const vk::WriteDescriptorSet writeDescriptorSet(
		descriptorSet,										//Descriptor set
		FINALIZATION_DESCRIPTOR_BINDING_INPUT_ATTACHMENT,	//Binding
		0,													//Array index
		1,													//Descriptor count
		vk::DescriptorType::eInputAttachment,				//Descriptor type
		&descriptorImageInfo,								//Image descriptors
		nullptr,											//Uniform buffer descriptors
		nullptr												//Texel buffers
	);

	vulkan.updateDescriptorSets(writeDescriptorSet, {});
}



vk::RenderPass RenderPass::createRenderPass(const Vulkan& vulkan, 
											Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
											vk::Format depthStencilFmt,
											vk::Format intermediaryFmt,
											vk::ImageLayout finalLayout )
{
	//Some type definitions and constants used for indexing
	using Field = uint;
	constexpr size_t INDEX_FIELD_COUNT = 8; //Actually 7, just in case 8
	constexpr size_t INDEX_FIELD_OFFSET = sizeof(Field) * Utils::getByteSize();
	using Index = std::bitset<INDEX_FIELD_OFFSET * INDEX_FIELD_COUNT>;

	//Obtain the index for these parameters
	Index index;

	//Add all the plane formats
	for(const auto& plane : planeDescriptors) {
		assert(plane.swizzle == vk::ComponentMapping());

		index <<= INDEX_FIELD_OFFSET;
		index |= static_cast<Field>(plane.format);
	}

	//Pad upto 4 plane formats and add the depth stencil format
	index <<= INDEX_FIELD_OFFSET * (4 - planeDescriptors.size() + 1);
	index |= static_cast<Field>(depthStencilFmt);

	//Add the intermediary format
	index <<= INDEX_FIELD_OFFSET;
	index |= static_cast<Field>(intermediaryFmt);

	//Add the final layout
	index <<= INDEX_FIELD_OFFSET;
	index |= static_cast<Field>(finalLayout);

	//Finally obtain the id
	static std::unordered_map<Index, const Utils::StaticId> ids; //TODO make thread-safe
	const auto& id = ids[index];

	//Check if a renderpass with this id is cached
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