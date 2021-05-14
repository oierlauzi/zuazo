#include <zuazo/Graphics/StagedFrame.h>

#include <zuazo/Graphics/Image.h>
#include <zuazo/Graphics/Sampler.h>
#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/WholeViewportTriangle.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/Utils/Hasher.h>

#include <unordered_map>
#include <tuple>

namespace Zuazo::Graphics {

/*
 * Format support
 */
static const std::vector<vk::Format>& getVulkanFormatSupportTransfer(const Vulkan& vulkan) {
	constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
		vk::FormatFeatureFlagBits::eSampledImage |
		vk::FormatFeatureFlagBits::eTransferDst ;

	return vulkan.listSupportedFormatsOptimal(DESIRED_FLAGS);
}

static const std::vector<vk::Format>& getVulkanFormatSupportRender(const Vulkan& vulkan) {
	constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
		vk::FormatFeatureFlagBits::eSampledImage |
		vk::FormatFeatureFlagBits::eColorAttachment ;

	return vulkan.listSupportedFormatsOptimal(DESIRED_FLAGS);
}



/*
 * StagedFrame::Cache
 */

class StagedFrame::Cache {
public:
	Cache(const Vulkan& vulkan, const Frame::Descriptor& desc)
		: m_vulkan(vulkan)
		, m_srcPlanes(getSourcePlanes(desc))
		, m_conversion()
		, m_dstPlane(getDestinationPlane(vulkan, desc, m_srcPlanes, m_conversion))
		, m_commandPool(createCommandPool(vulkan))
		, m_frameCache(Frame::createCache(vulkan, m_dstPlane))
	{
	}
	
	~Cache() = default;

	const Vulkan& getVulkan() const noexcept{
		return m_vulkan;
	}

	Utils::BufferView<const Image::Plane> getSourcePlanes() const noexcept {
		return m_srcPlanes;
	}

	const Image::Plane& getDestinationPlane() const noexcept {
		return m_dstPlane;
	}

	bool needsConversion() const noexcept {
		return static_cast<bool>(m_conversion);
	}

	const ColorTransferRead& getConversionColorTransfer() const noexcept {
		return m_conversion->colorTransfer;
	}

	Utils::BufferView<const Image::Plane> getConversionIntermediaryPlanes() const noexcept {
		return m_conversion->intPlanes;
	}

	const Sampler& getConversionSampler() const noexcept {
		return m_conversion->sampler;
	}

	vk::DescriptorSetLayout getConversionDescriptorSetLayout() const noexcept {
		return m_conversion->descriptorSetLayout;
	}

	vk::RenderPass getConversionRenderPass() const noexcept {
		return m_conversion->renderPass;
	}

	vk::PipelineLayout getConversionPipelineLayout() const noexcept {
		return m_conversion->pipelineLayout;
	}

	vk::Pipeline getConversionPipeline() const noexcept {
		return m_conversion->pipeline;
	}

	vk::CommandPool getCommandPool() const noexcept {
		return *m_commandPool;
	}

	const std::shared_ptr<const Frame::Cache>& 	getFrameCache() const noexcept {
		return m_frameCache;
	}

private:
	struct Conversion {
		Conversion(	const Vulkan& vulkan, 
					const Frame::Descriptor& frameDesc,
					const std::vector<Image::Plane>& srcPlane,
					const Image::Plane& dstPlane )
			: colorTransfer(frameDesc)
			, intPlanes(getIntermediaryPlanes(vulkan, srcPlane, colorTransfer))
			, sampler(createSampler(vulkan, frameDesc, intPlanes, colorTransfer))
			, descriptorSetLayout(createDescriptorSetLayout(vulkan, colorTransfer, sampler))
			, renderPass(createRenderPass(vulkan, dstPlane))
			, pipelineLayout(createPipelineLayout(vulkan, descriptorSetLayout))
			, pipeline(createPipeline(vulkan, dstPlane, renderPass, pipelineLayout, colorTransfer))
		{
		}
		~Conversion() = default;

		ColorTransferRead					colorTransfer;
		std::vector<Image::Plane>			intPlanes;
		Sampler								sampler;
		vk::DescriptorSetLayout				descriptorSetLayout;
		vk::RenderPass						renderPass;
		vk::PipelineLayout					pipelineLayout;
		vk::Pipeline						pipeline;

	private:
		static std::vector<Image::Plane> getIntermediaryPlanes(	const Vulkan& vulkan, 
																const std::vector<Image::Plane>& srcPlanes,
																ColorTransferRead& colorTransfer )
		{
			std::vector<Image::Plane> result = srcPlanes;

			//Try to optimize it
			const auto& supportedFormats = getVulkanFormatSupportTransfer(vulkan);
			colorTransfer.optimize(result, supportedFormats);

			//There may be less planes after the optimization. Erase the excess ones
			const auto ite = std::remove_if(
				result.begin(), result.end(),
				[] (const Image::Plane& plane) -> bool {
					return plane.getFormat() == vk::Format::eUndefined;
				}
			);
			result.erase(ite, result.end());

			//Try to remove the swizzle from the planes
			optimizeSwizzle(result, supportedFormats);

			return result;
		}

		static Sampler createSampler(	const Vulkan& vulkan,
										const Frame::Descriptor& frameDesc,
										const std::vector<Image::Plane>& intPlanes,
										ColorTransferRead& colorTransfer )
		{
			//Use linear filter only if subsampled
			const auto chromaReconstruction = frameDesc.getColorSubsampling() != ColorSubsampling::RB_444;
			const auto reconstructionFilter = 	chromaReconstruction ? 
												ScalingFilter::LINEAR : 
												ScalingFilter::NONE ;

			const Sampler result(
				vulkan,
				intPlanes.front(),
				frameDesc.getColorRange(),
				frameDesc.getColorModel(),
				frameDesc.getColorTransferFunction(),
				frameDesc.getColorChromaLocation(),
				reconstructionFilter
			);

			colorTransfer.optimize(result);

			return result;
		}

		static vk::RenderPass createRenderPass(	const Vulkan& vulkan, 
												const Image::Plane& dstPlane )
		{
			using Index = vk::Format;
			static std::unordered_map<Index, const Utils::StaticId, Utils::Hasher<Index>> ids;

			const Index index(dstPlane.getFormat());
			const auto& id = ids[index]; //TODO thread safe

			//Try to retrive the layout from cache
			auto result = vulkan.createRenderPass(id);
			if(!result) {
				const std::array<vk::AttachmentDescription, 1> attachments = {
					vk::AttachmentDescription(
						vk::AttachmentDescriptionFlags(),				//Flags
						dstPlane.getFormat(),							//Attachemnt format
						vk::SampleCountFlagBits::e1,					//Sample count
						vk::AttachmentLoadOp::eDontCare,				//Color attachment load operation
						vk::AttachmentStoreOp::eStore,					//Color attachemnt store operation
						vk::AttachmentLoadOp::eDontCare,				//Stencil attachment load operation
						vk::AttachmentStoreOp::eDontCare,				//Stencil attachment store operation
						vk::ImageLayout::eUndefined,					//Initial layout
						vk::ImageLayout::eShaderReadOnlyOptimal			//Final layout
					)
				};

				constexpr std::array<vk::AttachmentReference, 1> attachmentReferences = {
					vk::AttachmentReference(
						0,												//Index
						vk::ImageLayout::eColorAttachmentOptimal		//Layout
					)
				};

				const std::array<vk::SubpassDescription, 1> subpasses = {
					vk::SubpassDescription(
						{},															//Flags
						vk::PipelineBindPoint::eGraphics,							//Pipeline bind point
						0, nullptr,													//Input attachments
						attachmentReferences.size(), attachmentReferences.data(),	//Color attachments
						nullptr,													//Resolve attachment
						nullptr, 													//Depth-stencil attachment
						0, nullptr													//Preserve attachments
					)
				};

				const vk::RenderPassCreateInfo createInfo(
					{},														//Flags
					attachments.size(), attachments.data(),					//Attachments
					subpasses.size(), subpasses.data(), 					//Subpasses
					0, nullptr												//Dependencies (implicit ones)
				);

				result = vulkan.createRenderPass(id, createInfo);
			}
			assert(result);

			return result;
		}

		static vk::DescriptorSetLayout createDescriptorSetLayout(	const Vulkan& vulkan,
																	const ColorTransferRead& colorTransfer,
																	const Sampler& sampler )
		{
			return colorTransfer.createDescriptorSetLayout(vulkan, sampler);
		}

		static vk::PipelineLayout createPipelineLayout(	const Vulkan& vulkan,
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

		static vk::Pipeline createPipeline(	const Vulkan& vulkan,
											const Image::Plane& dstPlane,
											vk::RenderPass renderPass,
											vk::PipelineLayout pipelineLayout,
											const ColorTransferRead& colorTransfer ) 
		{
			using SpecializationData = std::array<uint64_t, 8>;
			using Index = std::tuple<	vk::RenderPass, 
										vk::PipelineLayout,
										uint32_t, uint32_t,
										SpecializationData >;

			static std::unordered_map<Index, const Utils::StaticId, Utils::Hasher<Index>> ids; //TODO make thread safe

			const auto extent = to2D(dstPlane.getExtent());

			//Copy the specialization data
			SpecializationData specData;
			assert(sizeof(specData) >= colorTransfer.size());
			std::memcpy(specData.data(), colorTransfer.data(), colorTransfer.size());

			//Obtain the id of the given parameters
			const Index index(
				renderPass,
				pipelineLayout,
				extent.width, extent.height,
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
					sizeof(specData), specData.data()
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

				//Set viewport an scissor
				const std::array<vk::Viewport, 1> viewports = {
					vk::Viewport(
						0.0f, 			0.0f,
						extent.width, 	extent.height,
						0.0f,			1.0f
					)
				};

				const std::array<vk::Rect2D, 1> scissors = {
					vk::Rect2D(
						vk::Offset2D(0, 0),
						extent
					)
				};

				const vk::PipelineViewportStateCreateInfo viewport(
					{},													//Flags
					viewports.size(), viewports.data(),					//Viewports (dynamic)
					scissors.size(), scissors.data()					//Scissors (dynamic)
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

				constexpr std::array<vk::PipelineColorBlendAttachmentState, 1> colorBlendAttachments = {
					Graphics::getBlendingConfiguration(BlendingMode::WRITE)
				};

				const vk::PipelineColorBlendStateCreateInfo colorBlend(
					{},													//Flags
					false,												//Enable logic operation
					vk::LogicOp::eCopy,									//Logic operation
					colorBlendAttachments.size(), colorBlendAttachments.data() //Blend attachments
				);

				const vk::PipelineDynamicStateCreateInfo dynamicState(
					{},													//Flags
					0, nullptr											//Dynamic states
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
					renderPass, 0,										//Renderpasses
					nullptr, 0											//Inherit
				);

				result = vulkan.createGraphicsPipeline(id, createInfo);
			}

			return result;
		}

	};

	std::reference_wrapper<const Vulkan>m_vulkan;
	std::vector<Image::Plane> 			m_srcPlanes;
	std::unique_ptr<Conversion>			m_conversion;
	Image::Plane						m_dstPlane;

	vk::UniqueCommandPool				m_commandPool;

	std::shared_ptr<const Frame::Cache>	m_frameCache;



	static std::vector<Image::Plane> getSourcePlanes(const Frame::Descriptor& frameDesc) {
		return frameDesc.getPlanes();
	}

	static Image::Plane getDestinationPlane(const Vulkan& vulkan, 
											const Frame::Descriptor& frameDesc,
											const std::vector<Image::Plane>& srcPlanes,
											std::unique_ptr<Conversion>& conversion )
	{
		//Initialize the result
		assert(srcPlanes.size() > 0);
		Image::Plane result = srcPlanes.front();

		bool requiresConversion;
		if(srcPlanes.size() > 1) {
			//Multi-planar formats require conversion
			requiresConversion = true;
		} else if(requiresYCbCrSamplerConversion(result.getFormat())) {
			//Special samplers also require conversion
			requiresConversion = true;
		} else {
			//Try to optimize the format. If the optimized format is not passthough, 
			//it should be converted. This step will consider optimizations related
			//to the sRGB transfer function
			const auto& supportedFormats = getVulkanFormatSupportTransfer(vulkan);
			ColorTransferRead colorTransfer(frameDesc);
			colorTransfer.optimize(result, supportedFormats);
			optimizeSwizzle(result, supportedFormats); //TODO Only needed if it turns out to not need conversion

			requiresConversion = !colorTransfer.isPassthough();
		}

		if(requiresConversion) {
			//No luck, we need to convert
			//Select the appropiate intermediate format
			const auto& supportedFormats = getVulkanFormatSupportRender(vulkan);
			const auto referenceFormat = srcPlanes.front().getFormat();
			const auto useAlpha = hasAlpha(frameDesc.getColorFormat());
			const auto format = getAdequateFloatingPointFormat(referenceFormat, useAlpha, supportedFormats);

			//Write the changes
			result.setFormat(format);
			result.setSwizzle(vk::ComponentMapping()); //Remove any swizzle

			//Create a conversion object
			conversion = Utils::makeUnique<Conversion>(vulkan, frameDesc, srcPlanes, result);
		}

		return result;
	}

	static vk::UniqueCommandPool createCommandPool(const Vulkan& vulkan) {
		const vk::CommandPoolCreateInfo createInfo(
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			vulkan.getTransferQueueIndex()
		);

		return vulkan.createCommandPool(createInfo);
	}

};





/*
 * StagedFrame::Impl
 */

struct StagedFrame::Impl {
	struct IntermediaryImage {
		IntermediaryImage(	const Vulkan& vulkan,
							const Image& dstImage,
							const Cache& cache  )
			: image(createImage(vulkan, cache))
			, framebuffer(createFramebuffer(vulkan, cache, dstImage))
			, descriptorPool(createDescriptorPool(vulkan, image))
			, descriptorSet(allocateDescriptorSet(vulkan, cache, image, *descriptorPool))
		{
		}

		Image						image;
		vk::UniqueFramebuffer		framebuffer;
		vk::UniqueDescriptorPool	descriptorPool;
		vk::DescriptorSet			descriptorSet;

	private:
		static Image createImage(	const Vulkan& vulkan,
									const Cache& cache )
		{
			constexpr vk::ImageUsageFlags usage = 
				vk::ImageUsageFlagBits::eTransferDst |
				vk::ImageUsageFlagBits::eSampled ;

			constexpr vk::ImageTiling tiling = vk::ImageTiling::eOptimal;

			constexpr vk::MemoryPropertyFlags memoryProperties = 
				vk::MemoryPropertyFlagBits::eDeviceLocal;
			
			return Image(
				vulkan,
				cache.getConversionIntermediaryPlanes(),
				usage,
				tiling,
				memoryProperties,
				&cache.getConversionSampler()
			);
		}

		static vk::UniqueFramebuffer createFramebuffer(	const Vulkan& vulkan,
														const Cache& cache,
														const Image& dstImage )
		{
			assert(dstImage.getPlanes().size() == 1);
			const auto& plane = dstImage.getPlanes().front();
			const auto& extent = plane.getExtent();

			const std::array<vk::ImageView, 1> attachments = {
				plane.getImageView()
			};

			const vk::FramebufferCreateInfo createInfo(
				{},
				cache.getConversionRenderPass(),
				attachments.size(),
				attachments.data(),
				extent.width, extent.height,
				1
			);

			return vulkan.createFramebuffer(createInfo);
		}

		static vk::UniqueDescriptorPool	createDescriptorPool(	const Vulkan& vulkan,
																const Image& image) 
		{
			//A descriptor pool is created, from which 1 descriptor set will
			//be allocated. This will hold at most 4 combined image samplers
			const std::array<vk::DescriptorPoolSize, 1> poolSizes = {
				vk::DescriptorPoolSize(
					vk::DescriptorType::eCombinedImageSampler,			//Descriptor type
					image.getPlanes().size()							//Descriptor count
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
														const Cache& cache,
														const Image& image,
														vk::DescriptorPool pool )
		{
			//Allocate the descriptor set
			auto result = vulkan.allocateDescriptorSet(
				pool, 
				cache.getConversionDescriptorSetLayout()
			);

			//Write the descriptor set's content
			const auto planes = image.getPlanes();
			const auto sampler = cache.getConversionSampler().getSampler();
			std::vector<vk::DescriptorImageInfo> descriptorImageInfos;
			descriptorImageInfos.reserve(planes.size());
			std::transform(
				planes.cbegin(), planes.cend(),
				std::back_inserter(descriptorImageInfos),
				[sampler] (const Image::Plane& plane) -> vk::DescriptorImageInfo {
					return vk::DescriptorImageInfo(
						sampler,											//Sampler (already set, redundant)
						plane.getImageView(),								//Image view
						vk::ImageLayout::eShaderReadOnlyOptimal				//Layout
					);
				}
			);

			const std::array<vk::WriteDescriptorSet, 1> writeDescriptorSets = {
				vk::WriteDescriptorSet( //Image descriptor
					*result,												//Descriptor set
					ColorTransferRead::getSamplerBinding(),					//Binding
					0, 														//Index
					descriptorImageInfos.size(), 							//Descriptor count
					vk::DescriptorType::eCombinedImageSampler,				//Descriptor type
					descriptorImageInfos.data(), 							//Images
					nullptr, 												//Buffers
					nullptr													//Texel buffers
				)
			};

			vulkan.updateDescriptorSets(Utils::BufferView<const vk::WriteDescriptorSet>(writeDescriptorSets));

			//Release it, as it will be freed when de pool is destroyed
			return result.release();
		}
	};

	std::shared_ptr<const Cache>				cache;

	Image										stagingImage;
	std::vector<Utils::BufferView<std::byte>> 	pixelData;

	std::unique_ptr<IntermediaryImage>			intermediaryImage;

	vk::UniqueCommandBuffer						commandBuffer;
	vk::SubmitInfo 								commandBufferSubmit;
	vk::UniqueFence								uploadComplete;


	Impl(	const Vulkan& vulkan,
			const Image& dstImage,
			std::shared_ptr<const Cache> c )
		: cache(std::move(c))
		, stagingImage(createStagingImage(vulkan, *cache))
		, pixelData(getPixelData(vulkan, stagingImage))
		, intermediaryImage(createIntermediaryImage(vulkan, dstImage, *cache))
		, commandBuffer(createCommandBuffer(vulkan, cache->getCommandPool()))
		, commandBufferSubmit(createSubmitInfo(*commandBuffer))
		, uploadComplete(vulkan.createFence(false))
	{
		transitionStagingImageLayout(vulkan);
		recordCommandBuffer(vulkan, dstImage);
	}

	~Impl() {
		waitCompletion(cache->getVulkan(), Vulkan::NO_TIMEOUT);
	}


	StagedFrame::PixelData getPixelData() noexcept {
		return pixelData;
	}

	StagedFrame::ConstPixelData getPixelData() const noexcept {
		return ConstPixelData(
			reinterpret_cast<const Utils::BufferView<const std::byte>*>(pixelData.data()),
			pixelData.size()
		);
	}


	void flush(const Vulkan& vulkan) {
		//There should not be any pending upload
		assert(waitCompletion(vulkan, 0));

		//Flush the mapped memory
		const vk::MappedMemoryRange range(
			*(stagingImage.getMemory().memory),
			0, VK_WHOLE_SIZE
		);
		vulkan.flushMappedMemory(range);

		//Send it to the queue
		vulkan.resetFences(*uploadComplete);
		vulkan.submit(
			vulkan.getTransferQueue(),
			commandBufferSubmit,
			*uploadComplete
		);
	}

	bool waitCompletion(const Vulkan& vulkan, uint64_t timeo) const {
		return vulkan.waitForFences(*uploadComplete, true, timeo);
	}



	static Frame createFrame(	const Vulkan& vulkan, 
								std::shared_ptr<const Frame::Descriptor> desc,
								std::shared_ptr<const Cache>& cache,
								std::shared_ptr<void> usrPtr )
	{
		//Ensure that the cache exists
		if(!cache) {
			cache = createCache(vulkan, *desc);
		}
		assert(cache);

		//Decide the usage based on wether if it is going to be 
		//converted or not
		const vk::ImageUsageFlags usage = 	cache->needsConversion() ?
											vk::ImageUsageFlagBits::eColorAttachment :
											vk::ImageUsageFlagBits::eTransferDst ;

		return Frame(
			vulkan,
			cache->getDestinationPlane(),
			usage,
			std::move(desc),
			cache->getFrameCache(),
			std::move(usrPtr)
		);
	}

	static std::shared_ptr<Cache> createCache(	const Vulkan& vulkan, 
												const Frame::Descriptor& frameDesc )
	{
		return Utils::makeShared<Cache>(vulkan, frameDesc);
	}

	static Utils::Discrete<ColorFormat> getSupportedFormats(const Vulkan& vulkan) {
		//TODO cache the result
		Utils::Discrete<ColorFormat> result;

		//Query support for Vulkan formats
		const auto& vulkanFormatSupport = getVulkanFormatSupportTransfer(vulkan);
		assert(std::is_sorted(vulkanFormatSupport.cbegin(), vulkanFormatSupport.cend())); //For binary search

		//Test for each format
		for(auto i = Utils::EnumTraits<ColorFormat>::first(); i <= Utils::EnumTraits<ColorFormat>::last(); ++i) {
			//Convert it into a Vulkan format
			const auto conversion = toVulkan(i);

			//Find the end of the range
			const auto endIte = std::find_if(
				conversion.cbegin(), conversion.cend(),
				[] (const std::tuple<vk::Format, vk::ComponentMapping>& conv) -> bool {
					return std::get<0>(conv) == vk::Format::eUndefined;
				}
			);

			//Check if it is supported
			const auto supported = std::all_of(
				conversion.cbegin(), endIte,
				[&vulkanFormatSupport] (const std::tuple<vk::Format, vk::ComponentMapping>& conv) -> bool {
					return std::binary_search(vulkanFormatSupport.cbegin(), vulkanFormatSupport.cend(), std::get<0>(conv));
				}
			);

			if(supported) {
				result.push_back(i);
			}
		}

		return result;
	}

private:
	void transitionStagingImageLayout(const Vulkan& vulkan) {
		const auto& image = stagingImage;
		const auto cmd = *commandBuffer;
		const size_t planeCount = image.getPlanes().size();

		constexpr vk::ImageSubresourceRange imageSubresourceRange(
			vk::ImageAspectFlagBits::eColor,				//Aspect mask
			0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
		);

		//Record the command buffer
		const vk::CommandBufferBeginInfo beginInfo(
			{},
			nullptr
		);

		vulkan.begin(cmd, beginInfo);

		std::vector<vk::ImageMemoryBarrier> memoryBarriers;
		memoryBarriers.reserve(planeCount);

		for(const auto& plane : image.getPlanes()){
			constexpr vk::AccessFlags srcAccess = {};
			constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eHostWrite;

			constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eUndefined;
			constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eGeneral;

			//Allways owned by the transfer queue
			constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
			constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

			memoryBarriers.emplace_back(
				srcAccess,								//Old access mask
				dstAccess,								//New access mask
				srcLayout,								//Old layout
				dstLayout,								//New layout
				srcFamily,								//Old queue family
				dstFamily,								//New queue family
				plane.getImage(),						//Image
				imageSubresourceRange					//Image subresource
			);
		}
		assert(memoryBarriers.size() == planeCount);

		constexpr vk::PipelineStageFlags srcStages =
			vk::PipelineStageFlagBits::eHost;

		constexpr vk::PipelineStageFlags dstStages = 
			vk::PipelineStageFlagBits::eHost;

		vulkan.pipelineBarrier(
			cmd,										//Command buffer
			srcStages,									//Generating stages
			dstStages,									//Consuming stages
			{},											//Dependency flags
			Utils::BufferView<const vk::ImageMemoryBarrier>(memoryBarriers) //Memory barriers
		);

		vulkan.end(cmd);

		//Submit it and wait until it completes execution (blocking)
		vulkan.submit(vulkan.getTransferQueue(), commandBufferSubmit, *uploadComplete);
		waitCompletion(vulkan, Vulkan::NO_TIMEOUT);
	}

	void recordCommandBuffer(const Vulkan& vulkan, const Image& dstImage) {
		const auto& srcImage = stagingImage;
		const auto cmd = *commandBuffer;

		//Record the command buffer
		const vk::CommandBufferBeginInfo beginInfo(
			{},
			nullptr
		);

		vulkan.begin(cmd, beginInfo);

		//Upload the image
		uploadImage(
			vulkan, 
			cmd,
			srcImage,
			intermediaryImage ? intermediaryImage->image : dstImage
		);

		//Convert if necessary
		if(intermediaryImage) {
			convertImage(
				vulkan,
				cmd,
				*intermediaryImage,
				dstImage,
				*cache
			);
		}

		//Convert if necessary


		vulkan.end(cmd);
	}

	static void uploadImage(const Vulkan& vulkan,
							vk::CommandBuffer cmd,
							const Image& srcImage,
							const Image& dstImage )
	{
		constexpr vk::ImageSubresourceRange imageSubresourceRange(
			vk::ImageAspectFlagBits::eColor,				//Aspect mask
			0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
		);

		const bool queueOwnershipTransfer = vulkan.getGraphicsQueueIndex() != vulkan.getTransferQueueIndex();
		const size_t barrierCount = srcImage.getPlanes().size() + dstImage.getPlanes().size();

		std::vector<vk::ImageMemoryBarrier> memoryBarriers;
		memoryBarriers.reserve(barrierCount);

		//Transition the layout of the images
		{
			memoryBarriers.clear();
			for(const auto& plane : srcImage.getPlanes()){
				constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eHostWrite;
				constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferRead;

				constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eGeneral;
				constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eTransferSrcOptimal;

				//Allways owned by the transfer queue
				constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
				constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

				memoryBarriers.emplace_back(
					srcAccess,								//Old access mask
					dstAccess,								//New access mask
					srcLayout,								//Old layout
					dstLayout,								//New layout
					srcFamily,								//Old queue family
					dstFamily,								//New queue family
					plane.getImage(),						//Image
					imageSubresourceRange					//Image subresource
				);
			}
			for(const auto& plane : dstImage.getPlanes()){
				constexpr vk::AccessFlags srcAccess = {};
				constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferWrite;

				constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eUndefined;
				constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eTransferDstOptimal;

				//We don't mind about the previous contents, so skip the ownership transfer
				constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
				constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

				memoryBarriers.emplace_back(
					srcAccess,								//Old access mask
					dstAccess,								//New access mask
					srcLayout,								//Old layout
					dstLayout,								//New layout
					srcFamily,								//Old queue family
					dstFamily,								//New queue family
					plane.getImage(),						//Image
					imageSubresourceRange					//Image subresource
				);
			}
			assert(memoryBarriers.size() == barrierCount);

			constexpr vk::PipelineStageFlags srcStages = 
				vk::PipelineStageFlagBits::eTopOfPipe |
				vk::PipelineStageFlagBits::eHost ;

			constexpr vk::PipelineStageFlags dstStages = 
				vk::PipelineStageFlagBits::eTransfer;

			vulkan.pipelineBarrier(
				cmd,										//Command buffer
				srcStages,									//Generating stages
				dstStages,									//Consuming stages
				{},											//Dependency flags
				Utils::BufferView<const vk::ImageMemoryBarrier>(memoryBarriers) //Memory barriers
			);
		}

		//Copy the image to the image
		copy(vulkan, cmd, srcImage, const_cast<Image&>(dstImage)); //FIXME ugly const_cast

		//Transition the layout of the images (again)
		{
			memoryBarriers.clear();
			for(const auto& plane : srcImage.getPlanes()){
				constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferRead;
				constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eHostWrite;

				constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eTransferSrcOptimal;
				constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eGeneral;
				
				//Allways owned by the transfer queue
				constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
				constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

				memoryBarriers.emplace_back(
					srcAccess,								//Old access mask
					dstAccess,								//New access mask
					srcLayout,								//Old layout
					dstLayout,								//New layout
					srcFamily,								//Old queue family
					dstFamily,								//New queue family
					plane.getImage(),						//Image
					imageSubresourceRange					//Image subresource
				);
			}
			for(const auto& plane : dstImage.getPlanes()){
				constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferWrite;
				constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eShaderRead;

				constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eTransferDstOptimal;
				constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

				//Handle ownership back to the graphics queue
				const auto srcFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
				const auto dstFamily = queueOwnershipTransfer ? vulkan.getGraphicsQueueIndex() : VK_QUEUE_FAMILY_IGNORED;

				memoryBarriers.emplace_back(
					srcAccess,								//Old access mask
					dstAccess,								//New access mask
					srcLayout,								//Old layout
					dstLayout,								//New layout
					srcFamily,								//Old queue family
					dstFamily,								//New queue family
					plane.getImage(),						//Image
					imageSubresourceRange					//Image subresource
				);
			}
			assert(memoryBarriers.size() == barrierCount);

			constexpr vk::PipelineStageFlags srcStages = 
				vk::PipelineStageFlagBits::eTransfer;

			constexpr vk::PipelineStageFlags dstStages = 
				vk::PipelineStageFlagBits::eAllGraphics |
				vk::PipelineStageFlagBits::eHost ;

			vulkan.pipelineBarrier(
				cmd,										//Command buffer
				srcStages,									//Generating stages
				dstStages,									//Consuming stages
				{},											//Dependency flags
				Utils::BufferView<const vk::ImageMemoryBarrier>(memoryBarriers) //Memory barriers
			);
		}
	}

	static void convertImage(	const Vulkan& vulkan,
								vk::CommandBuffer cmd,
								const IntermediaryImage& intImage,
								const Image& dstImage,
								const Cache& cache ) 
	{
		const auto extent = to2D(dstImage.getPlanes().front().getExtent());

		//Begin a renderpass
		const vk::RenderPassBeginInfo beginInfo(
			cache.getConversionRenderPass(),			//Renderpass
			*intImage.framebuffer,						//Framebuffer
			vk::Rect2D(vk::Offset2D(), extent),			//Render area
			{}											//Clear values
		);
		vulkan.beginRenderPass(cmd, beginInfo, vk::SubpassContents::eInline);

		//Bind the descriptors and the pipeline
		vulkan.bindDescriptorSets(
			cmd,									//Command buffer
			vk::PipelineBindPoint::eGraphics,		//Pipeline bind point
			cache.getConversionPipelineLayout(),	//Pipeline layout
			0,										//First index
			intImage.descriptorSet,					//Descriptor sets
			{}										//Dynamic offsets
		);
		vulkan.bindPipeline(
			cmd, 									//Command buffer
			vk::PipelineBindPoint::eGraphics, 		//Pipeline bind point
			cache.getConversionPipeline()			//Pipeline
		);

		//Draw a fullscreen triangle
		vulkan.draw(cmd, 3, 1, 0, 0);

		//Finish the renderpass
		vulkan.endRenderPass(cmd);
	}

	static Image createStagingImage(const Vulkan& vulkan, 
									const Cache& cache )
	{
		constexpr vk::ImageUsageFlags usage = 
			vk::ImageUsageFlagBits::eTransferSrc;

		constexpr vk::ImageTiling tiling = 
			vk::ImageTiling::eLinear;

		constexpr vk::MemoryPropertyFlags memory = 
			vk::MemoryPropertyFlagBits::eHostVisible;

		return Image(
			vulkan,
			cache.getSourcePlanes(),
			usage,
			tiling,
			memory,
			nullptr
		);
	}
	
	static std::vector<Utils::BufferView<std::byte>> getPixelData(	const Vulkan& vulkan,
																	const Image& stagingImage )
	{
		const vk::MappedMemoryRange range(
			*(stagingImage.getMemory().memory),
			0, VK_WHOLE_SIZE
		);

		return Utils::slice(
			vulkan.mapMemory(range), 
			stagingImage.getMemory().areas
		);
	}

	static std::unique_ptr<IntermediaryImage> createIntermediaryImage(	const Vulkan& vulkan, 
																		const Image& dstImage,
																		const Cache& cache )
	{
		std::unique_ptr<IntermediaryImage> result;

		if(cache.needsConversion()) {
			result = Utils::makeUnique<IntermediaryImage>(
				vulkan,
				dstImage,
				cache
			);
		}

		return result;
	}

	static vk::UniqueCommandBuffer createCommandBuffer(	const Vulkan& vulkan,
														vk::CommandPool cmdPool )
	{
		return vulkan.allocateCommnadBuffer(cmdPool, vk::CommandBufferLevel::ePrimary);
	}

	static vk::SubmitInfo createSubmitInfo(const vk::CommandBuffer& cmdBuffer) {
		return vk::SubmitInfo(
			0, nullptr,							//Wait semaphores
			nullptr,							//Pipeline stages
			1, &cmdBuffer,						//Command buffers
			0, nullptr							//Signal semaphores
		);
	}

};





/*
 * StagedFrame
 */

StagedFrame::StagedFrame(	const Vulkan& vulkan,
							std::shared_ptr<const Descriptor> desc,
							std::shared_ptr<const Cache> cache,
							std::shared_ptr<void> usrPtr  )
	: Frame(Impl::createFrame(vulkan, std::move(desc), cache, std::move(usrPtr)))
	, m_impl({}, vulkan, getImage(), std::move(cache))
{
}

StagedFrame::StagedFrame(StagedFrame&& other) noexcept = default;

StagedFrame::~StagedFrame() = default;

StagedFrame& StagedFrame::operator=(StagedFrame&& other) noexcept = default;



StagedFrame::PixelData StagedFrame::getPixelData() noexcept {
	return m_impl->getPixelData();
}

StagedFrame::ConstPixelData StagedFrame::getPixelData() const noexcept {
	return m_impl->getPixelData();
}

void StagedFrame::flush() {
	m_impl->flush(getVulkan());
}

bool StagedFrame::waitCompletion(uint64_t timeo) const {
	return m_impl->waitCompletion(getVulkan(), timeo);
}



std::shared_ptr<const StagedFrame::Cache> StagedFrame::createCache(	const Vulkan& vulkan, 
																	const Frame::Descriptor& frameDesc )
{
	return Impl::createCache(vulkan, frameDesc);
}

Utils::Discrete<ColorFormat> StagedFrame::getSupportedFormats(const Vulkan& vulkan) {
	return Impl::getSupportedFormats(vulkan);
}

}