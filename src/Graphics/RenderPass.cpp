#include <zuazo/Graphics/RenderPass.h>

#include <zuazo/Graphics/WholeViewportTriangle.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/Utils/Hasher.h>

#include <unordered_map>
#include <algorithm>
#include <bitset>

namespace Zuazo::Graphics {

/*
 * RenderPass::Impl
 */

struct RenderPass::Impl {
	struct Conversion {
		Conversion(	const Vulkan& vulkan,
					const ColorTransferWrite& colorTransfer,
					Utils::BufferView<const Image::Plane> planeDescriptors,
					vk::Format intermediaryImageFmt,
					vk::RenderPass renderPass )
			: intermediaryImage(createIntermediaryImage(vulkan, planeDescriptors, intermediaryImageFmt))
			, descriptorPool(createDescriptorPool(vulkan))
			, descriptorSetLayout(colorTransfer.createDescriptorSetLayout(vulkan))
			, descriptorSet(allocateDescriptorSet(vulkan, intermediaryImage, *descriptorPool, descriptorSetLayout))
			, pipelineLayout(createFinalizationPipelineLayout(vulkan, descriptorSetLayout))
			, pipeline(createFinalizationPipeline(vulkan, renderPass, pipelineLayout, colorTransfer))
		{
		}


		~Conversion() = default;

		Image								intermediaryImage;

		vk::UniqueDescriptorPool			descriptorPool;
		vk::DescriptorSetLayout				descriptorSetLayout;
		vk::DescriptorSet					descriptorSet;

		vk::PipelineLayout					pipelineLayout;
		vk::Pipeline						pipeline;

	private:
		static Image createIntermediaryImage(	const Vulkan& vulkan,
												Utils::BufferView<const Image::Plane> planeDescriptors,
												vk::Format intermediaryImageFmt )
		{
			const Image::Plane plane(
				planeDescriptors.front().getExtent(),
				intermediaryImageFmt,
				vk::ComponentMapping()
			);

			constexpr vk::ImageUsageFlags usage =
				vk::ImageUsageFlagBits::eColorAttachment |
				vk::ImageUsageFlagBits::eInputAttachment |
				vk::ImageUsageFlagBits::eTransientAttachment ;

			constexpr vk::ImageTiling tiling = vk::ImageTiling::eOptimal;

			constexpr vk::MemoryPropertyFlags memory =
				vk::MemoryPropertyFlagBits::eDeviceLocal |
				vk::MemoryPropertyFlagBits::eLazilyAllocated ;

			return Image(
				vulkan,
				plane,
				usage,
				tiling,
				memory,
				nullptr
			);
		}

		static vk::UniqueDescriptorPool createDescriptorPool(const Vulkan& vulkan)
		{
			//A descriptor pool is created, from which 1 descriptor sets will
			//be allocated. This will hold 1 input attachment
			const std::array<vk::DescriptorPoolSize, 1> poolSizes = {
				vk::DescriptorPoolSize(
					vk::DescriptorType::eInputAttachment,				//Descriptor type
					1													//Descriptor count
				)
			};

			const vk::DescriptorPoolCreateInfo createInfo(
				{},														//Flags
				1,														//Descriptor set count
				poolSizes.size(), poolSizes.data()						//Pool sizes
			);

			return vulkan.createDescriptorPool(createInfo);
		}

		static vk::DescriptorSet allocateDescriptorSet(	const Vulkan& vulkan,
														const Image& image,
														vk::DescriptorPool pool,
														vk::DescriptorSetLayout descriptorSetLayout )
		{
			//Release the newly allocated descriptor set as it will be freed
			//when de pool gets freed
			auto result = vulkan.allocateDescriptorSet(
				pool, 
				descriptorSetLayout
			);

			const auto imageView = image.getPlanes().front().getImageView();
			const std::array<vk::DescriptorImageInfo, 1> descriptorImageInfos = {
				vk::DescriptorImageInfo(
					nullptr,											//Sampler
					imageView,											//Image view
					vk::ImageLayout::eShaderReadOnlyOptimal				//Layout
				)
			};

			const std::array<vk::WriteDescriptorSet, 1> writeDescriptorSets = {
				vk::WriteDescriptorSet( //Image descriptor
					*result,												//Descriptor set
					ColorTransferWrite::getInputAttachmentBinding(),		//Binding
					0, 														//Index
					descriptorImageInfos.size(), 							//Descriptor count
					vk::DescriptorType::eInputAttachment,					//Descriptor type
					descriptorImageInfos.data(), 							//Images
					nullptr, 												//Buffers
					nullptr													//Texel buffers
				)
			};

			vulkan.updateDescriptorSets(Utils::BufferView<const vk::WriteDescriptorSet>(writeDescriptorSets));

			//Release it, as it will be freed when de pool is destroyed
			return result.release();
		}

		static vk::PipelineLayout createFinalizationPipelineLayout(	const Vulkan& vulkan,
																	vk::DescriptorSetLayout descriptorSetLayout )
		{
			static std::unordered_map<vk::DescriptorSetLayout, const Utils::StaticId> ids;
			const auto& id = ids[descriptorSetLayout]; //TODO thread safe

			//Try to retrive the layout from cache
			auto result = vulkan.createPipelineLayout(id);
			if(!result) {
				const std::array<vk::DescriptorSetLayout, 1> descriptorSetLayouts = {
					descriptorSetLayout
				};

				const vk::PipelineLayoutCreateInfo createInfo(
					{},												//Flags
					descriptorSetLayouts.size(), descriptorSetLayouts.data(),	//Descriptor sets
					0, nullptr										//Push constants
				);

				result = vulkan.createPipelineLayout(id, createInfo);
			}
			assert(result);

			return result;
		}

		static vk::Pipeline createFinalizationPipeline(	const Graphics::Vulkan& vulkan,
														vk::RenderPass renderPass,
														vk::PipelineLayout pipelineLayout,
														const ColorTransferWrite& colorTransfer )
		{
			using SpecializationData = std::array<uint64_t, 8>;
			using Index = std::tuple<	vk::RenderPass, 
										vk::PipelineLayout,
										SpecializationData >;

			static std::unordered_map<Index, const Utils::StaticId, Utils::Hasher<Index>> ids; //TODO make thread safe

			//Copy the specialization data
			SpecializationData specData;
			assert(specData.size() >= colorTransfer.size());
			std::memcpy(specData.data(), colorTransfer.data(), colorTransfer.size());

			//Obtain the id of the given parameters
			const Index index(
				renderPass,
				pipelineLayout,
				specData
			);
			const auto& id = ids[index];

			auto result = vulkan.createGraphicsPipeline(id);
			if(!result) {
				static //So that its ptr can be used as an identifier
				const auto vertexShaderSPIRV = getWholeViewportTriangleSPIRV();
				const size_t vertId = reinterpret_cast<uintptr_t>(vertexShaderSPIRV.data());
				
				const auto fragmentShaderSPIRV = colorTransfer.getSPIRV();
				const size_t fragId = reinterpret_cast<uintptr_t>(fragmentShaderSPIRV.data());

				//Try to retrive shader modules from cache
				auto vertexShader = vulkan.createShaderModule(vertId);
				if(!vertexShader) {
					//Module isn't in cache. Create it
					vertexShader = vulkan.createShaderModule(vertId, vertexShaderSPIRV);
				}

				auto fragmentShader = vulkan.createShaderModule(fragId);
				if(!fragmentShader) {
					//Module isn't in cache. Create it
					fragmentShader = vulkan.createShaderModule(fragId, fragmentShaderSPIRV);
				}

				assert(vertexShader);
				assert(fragmentShader);


				//Specialization constants
				const auto fragmentShaderSpecializationMap = colorTransfer.getSpecializationMap();
				const vk::SpecializationInfo fragmentShaderSpecialization(
					fragmentShaderSpecializationMap.size(), fragmentShaderSpecializationMap.data(),
					specData.size(), specData.data()
				);

				constexpr auto SHADER_ENTRY_POINT = "main";
				const std::array shaderStages = {
					vk::PipelineShaderStageCreateInfo(		
						{},												//Flags
						vk::ShaderStageFlagBits::eVertex,				//Shader type
						vertexShader,									//Shader handle
						SHADER_ENTRY_POINT,								//Shader entry point
						nullptr											//Specialization constants
					),							
					vk::PipelineShaderStageCreateInfo(		
						{},												//Flags
						vk::ShaderStageFlagBits::eFragment,				//Shader type
						fragmentShader,									//Shader handle
						SHADER_ENTRY_POINT,								//Shader entry point
						&fragmentShaderSpecialization					//Specialization constants
					),	
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
					colorTransfer.getPlaneCount(),
					Graphics::getBlendingConfiguration(BlendingMode::write)
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
					nullptr, 0											//Inherit
				);

				result = vulkan.createGraphicsPipeline(id, createInfo);
			}

			return result;
		}

	};


	vk::Format							intermediaryFmt;
	vk::RenderPass						renderPass;
	std::unique_ptr<Image>				depthStencil;
	std::unique_ptr<Conversion>			conversion;

	Impl()
		: intermediaryFmt(vk::Format::eUndefined)
		, renderPass(nullptr)
		, depthStencil(nullptr)
		, conversion(nullptr)
	{
	}

	Impl(	const Vulkan& vulkan, 
			const ColorTransferWrite& colorTransfer,
			Utils::BufferView<const Image::Plane> planeDescriptors,
			DepthStencilFormat depthStencilFmt,
			vk::ImageLayout finalLayout )
		: intermediaryFmt(getIntermediateFormat(vulkan, planeDescriptors, colorTransfer))
		, renderPass(createRenderPass(vulkan, planeDescriptors, intermediaryFmt, toVulkan(depthStencilFmt), finalLayout))
		, depthStencil(createDepthStencil(vulkan, planeDescriptors, depthStencilFmt))
		, conversion(createConversion(vulkan, colorTransfer, planeDescriptors, intermediaryFmt, renderPass))
	{
	}

	~Impl() = default;



	vk::RenderPass get() const noexcept {
		return renderPass;
	}

	vk::UniqueFramebuffer createFramebuffer(const Vulkan& vulkan, const Image& target) const {
		const auto* depthStencilImage = depthStencil.get();
		const auto* intermediaryImage = conversion ? &conversion->intermediaryImage : nullptr;

		assert(target.getPlanes().size() > 0);
		const auto attachmentCount = 	target.getPlanes().size() + 
										static_cast<bool>(intermediaryImage) + 
										static_cast<bool>(depthStencilImage) ;
		const auto extent = target.getPlanes().front().getExtent();

		//Enumerate all attachments
		std::vector<vk::ImageView> attachments;
		attachments.reserve(attachmentCount);

		//First comes the depth image, if present
		if(depthStencilImage) {
			assert(depthStencilImage->getPlanes().size() == 1);
			attachments.emplace_back(depthStencilImage->getPlanes().front().getImageView());
		}

		//Then comes the intermediary image, if present
		if(intermediaryImage) {
			assert(intermediaryImage->getPlanes().size() == 1);
			attachments.emplace_back(intermediaryImage->getPlanes().front().getImageView());
		}

		//Finally comes the result image
		std::transform(
			target.getPlanes().cbegin(), target.getPlanes().cend(),
			std::back_inserter(attachments),
			[] (const Image::Plane& plane) -> vk::ImageView {
				return plane.getImageView();
			}
		);

		//Ensure that the size matches
		assert(attachments.size() == attachmentCount);

		//Create the FBO accordingly
		const vk::FramebufferCreateInfo createInfo(
			{},
			renderPass,
			attachments.size(),
			attachments.data(),
			extent.width, extent.height,
			1
		);

		return vulkan.createFramebuffer(createInfo);
	}

	void finalize(	const Vulkan& vulkan, 
					vk::CommandBuffer cmd ) const noexcept
	{
		if(conversion) {
			vulkan.nextSubpass(cmd, vk::SubpassContents::eInline);

			vulkan.bindDescriptorSets(
				cmd,									//Command buffer
				vk::PipelineBindPoint::eGraphics,		//Pipeline bind point
				conversion->pipelineLayout,				//Pipeline layout
				0,										//First index
				conversion->descriptorSet,				//Descriptor sets
				{}										//Dynamic offsets
			);

			vulkan.bindPipeline(
				cmd, 									//Command buffer
				vk::PipelineBindPoint::eGraphics, 		//Pipeline bind point
				conversion->pipeline					//Pipeline
			);

			//Draw a fullscreen triangle
			vulkan.draw(cmd, 3, 1, 0, 0);
		}
	}

	static Utils::BufferView<const vk::ClearValue> getClearValues(DepthStencilFormat depthStencilFmt) {
		Utils::BufferView<const vk::ClearValue> result;

		if(Math::isInRangeExclusive(depthStencilFmt, DepthStencilFormat::none, DepthStencilFormat::count)) {
			//First attachment is the depth/stencil
			//The next attachment is the intermediary or result attachment
			static const std::array<vk::ClearValue, 2> cv = {
				vk::ClearDepthStencilValue(1.0f, 0x00),
				vk::ClearColorValue()
			};

			result = cv;
		} else {
			//Only the intermediary or result attachment
			static const std::array<vk::ClearValue, 1> cv = {
				vk::ClearColorValue()
			};

			result = cv;
		}

		return result;
	}


private:
	static vk::Format getIntermediateFormat(const Vulkan& vulkan, 
											Utils::BufferView<const Image::Plane> planes,
											const ColorTransferWrite& colorTransfer )
	{
		auto result = vk::Format::eUndefined;
		const auto referenceFormat = planes.front().getFormat();

		if(!colorTransfer.isPassthough() && referenceFormat != vk::Format::eUndefined) {
			//Obtain the format support
			constexpr vk::FormatFeatureFlags formatFeatures =
				vk::FormatFeatureFlagBits::eColorAttachment |
				vk::FormatFeatureFlagBits::eColorAttachmentBlend ; //TODO input attachment feature?
			const auto& formatSupport = vulkan.listSupportedFormatsOptimal(formatFeatures);

			//Obtain the destination. //TODO query alpha requirement
			result = getAdequateFloatingPointFormat(referenceFormat, true, formatSupport);
		}

		return result;
	}

	static vk::RenderPass createRenderPass(	const Vulkan& vulkan, 
											Utils::BufferView<const Image::Plane> planeDescriptors,
											vk::Format intermediaryFmt,
											vk::Format depthStencilFmt,
											vk::ImageLayout finalLayout )
	{
		constexpr size_t MAX_PLANE_COUNT = 4;
		assert(MAX_PLANE_COUNT >= planeDescriptors.size());

		using Index = std::tuple<	std::array<vk::Format, MAX_PLANE_COUNT>,
									vk::Format,
									vk::Format,
									vk::ImageLayout >;

		//Calculate a index for these parameters
		std::array<vk::Format, MAX_PLANE_COUNT> planeFormats;
		assert(planeDescriptors.size() <= planeFormats.size());
		const auto lastPlaneFormat = std::transform(
			planeDescriptors.cbegin(), planeDescriptors.cend(),
			planeFormats.begin(),
			[] (const Image::Plane& plane) -> vk::Format {
				//In order to be a render target, it must not have a swizzle
				assert(plane.getSwizzle() == vk::ComponentMapping());
				return plane.getFormat();
			}
		);
		std::fill(lastPlaneFormat, planeFormats.end(), vk::Format::eUndefined);
		const Index index(
			planeFormats,
			intermediaryFmt,
			depthStencilFmt,
			finalLayout
		);

		//Obtain the id of the parameters
		static std::unordered_map<Index, const Utils::StaticId, Utils::Hasher<Index>> ids; 
		const auto& id = ids[index]; //TODO make thread-safe

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
					plane.getFormat(),								//Attachemnt format
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
			colorAttachmentReferences.reserve(MAX_PLANE_COUNT); //Worst case scenario
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

	std::unique_ptr<Image> createDepthStencil(	const Vulkan& vulkan,
												Utils::BufferView<const Image::Plane> planeDescriptors,
												DepthStencilFormat depthStencilFmt )
	{
		std::unique_ptr<Image> result;

		if(Math::isInRangeExclusive(depthStencilFmt, DepthStencilFormat::none, DepthStencilFormat::count)) {
			const Image::Plane plane(
				planeDescriptors.front().getExtent(),
				toVulkan(depthStencilFmt),
				vk::ComponentMapping()
			);

			constexpr vk::ImageUsageFlags usage =
				vk::ImageUsageFlagBits::eDepthStencilAttachment |
				vk::ImageUsageFlagBits::eTransientAttachment ;

			constexpr vk::ImageTiling tiling = vk::ImageTiling::eOptimal;

			constexpr vk::MemoryPropertyFlags memory =
				vk::MemoryPropertyFlagBits::eDeviceLocal |
				vk::MemoryPropertyFlagBits::eLazilyAllocated ;

			result = Utils::makeUnique<Image>(
				vulkan,
				plane,
				usage,
				tiling,
				memory,
				nullptr
			);
		}

		return result;
	}

	std::unique_ptr<Conversion> createConversion(	const Vulkan& vulkan,
													const ColorTransferWrite& colorTransfer,
													Utils::BufferView<const Image::Plane> planeDescriptors,
													vk::Format intermediaryImageFmt,
													vk::RenderPass renderPass )
	{
		std::unique_ptr<Conversion> result;

		if(intermediaryImageFmt != vk::Format::eUndefined) {
			result = Utils::makeUnique<Conversion>(
				vulkan, 
				colorTransfer, 
				planeDescriptors,
				intermediaryImageFmt,
				renderPass
			);
		}

		return result;
	}

};





/*
 * RenderPass
 */

const RenderPass RenderPass::NO_RENDERPASS;



RenderPass::RenderPass()
	: m_impl({})
{
}

RenderPass::RenderPass(	const Vulkan& vulkan, 
						const ColorTransferWrite& colorTransfer,
						Utils::BufferView<const Image::Plane> planeDescriptors,
						DepthStencilFormat depthStencilFmt,
						vk::ImageLayout finalLayout )
	: m_impl({}, vulkan, colorTransfer, planeDescriptors, depthStencilFmt, finalLayout)
{
}

RenderPass::RenderPass(RenderPass&& other) noexcept = default;

RenderPass::~RenderPass() = default;

RenderPass& RenderPass::operator=(RenderPass&& other) noexcept = default;

vk::RenderPass RenderPass::get() const noexcept {
	return m_impl->get();
}


vk::UniqueFramebuffer RenderPass::createFramebuffer(const Vulkan& vulkan, 
													const Image& target) const
{
	return m_impl->createFramebuffer(vulkan, target);
}

void RenderPass::finalize(	const Vulkan& vulkan, 
							vk::CommandBuffer cmd ) const noexcept
{
	return m_impl->finalize(vulkan, cmd);
}

Utils::BufferView<const vk::ClearValue> RenderPass::getClearValues(DepthStencilFormat depthStencilFmt) {
	return Impl::getClearValues(depthStencilFmt);
}

}