#include <Outputs/Window.h>

#include "../Graphics/VulkanUtils.h"

#include <Graphics/GLFW.h>
#include <Graphics/Vulkan.h>
#include <Graphics/VulkanConversions.h>
#include <Graphics/ColorTransfer.h>
#include <Graphics/Uploader.h>
#include <Utils/StaticId.h>

#include <cmath>
#include <algorithm>
#include <limits>
#include <set>

namespace Zuazo::Outputs {

/*
 * Window::Impl
 */
struct Window::Impl {
	static constexpr uint64_t NO_TIMEOUT = std::numeric_limits<uint64_t>::max();

	static constexpr uint32_t UBO_BINDING = 0;
	static constexpr uint32_t SHADER_SAMPLER_BINDING = 0;

	static constexpr uint32_t VERTEX_BUFFER_BINDING = 0;
	static constexpr uint32_t VERTEX_POSITION = 0;
	static constexpr uint32_t VERTEX_TEXCOORD = 1;



	struct Open {
		const Graphics::Vulkan&						vulkan;
		Graphics::GLFW::Window						window;
		vk::Extent2D								extent;
		Graphics::Frame::Geometry					geometry;
		vk::UniqueSurfaceKHR						surface;
		vk::UniqueCommandPool						commandPool;
		vk::CommandBuffer							commandBuffer;
		vk::UniqueSemaphore 						imageAvailableSemaphore;
		vk::UniqueSemaphore							renderFinishedSemaphore;
		vk::UniqueFence								renderFinishedFence;

		vk::UniqueSwapchainKHR						swapchain;
		std::vector<vk::UniqueImageView>			swapchainImageViews;
		vk::UniqueRenderPass						renderPass;
		std::vector<vk::UniqueFramebuffer>			framebuffers;
		vk::PipelineLayout							pipelineLayout;
		vk::UniquePipeline							pipeline;

	
		Open(	const Graphics::Vulkan& vulkan,
				Math::Vec2i size,
				std::string_view name,
				vk::Format format,
				vk::ColorSpaceKHR colorSpace,
				const Graphics::ColorTransfer& colorTransfer ) 
			: vulkan(vulkan)
			, window(size, name)
			, extent(Graphics::toVulkan(window.getResolution()))
			, geometry(vulkan, VERTEX_BUFFER_BINDING, Math::Vec2f(extent.width, extent.height))
			, surface(window.getSurface(vulkan))
			, commandPool(createCommandPool(vulkan))
			, commandBuffer(createCommandBuffer(vulkan, *commandPool))
			, imageAvailableSemaphore(vulkan.createSemaphore())
			, renderFinishedSemaphore(vulkan.createSemaphore())
			, renderFinishedFence(vulkan.createFence(true))
			, swapchain(createSwapchain(vulkan, *surface, extent, format, colorSpace, {}))
			, swapchainImageViews(createImageViews(vulkan, *swapchain, format))
			, renderPass(createRenderPass(vulkan, format))
			, framebuffers(createFramebuffers(vulkan, *renderPass, swapchainImageViews, extent))
			, pipelineLayout(createPipelineLayout(vulkan, vk::Filter::eNearest))
			, pipeline(createPipeline(vulkan, geometry, *renderPass, pipelineLayout, extent)) 

		{
		}

		~Open() {
			//Wait until the rendering has finished
			vulkan.getDevice().waitForFences(	*renderFinishedFence, 
												true, 
												NO_TIMEOUT, 
												vulkan.getDispatcher() );
		}

		void recreateSwapchain(	vk::Format format,
								vk::ColorSpaceKHR colorSpace ) 
		{
			swapchain = createSwapchain(vulkan, *surface, extent, format, colorSpace, *swapchain);
			swapchainImageViews = createImageViews(vulkan, *swapchain, format);
			renderPass = createRenderPass(vulkan, format);
			framebuffers = createFramebuffers(vulkan, *renderPass, swapchainImageViews, extent);
			pipeline = createPipeline(vulkan, geometry, *renderPass, pipelineLayout, extent);
		}
	};

	std::unique_ptr<Open>						opened;

	Impl() = default;
	~Impl() = default;

	void open(	const Graphics::Vulkan& vulkan, 
				const VideoMode& videoMode, 
				std::string_view name ) 
	{
		//Check pixel aspect ratio support
		if(videoMode.pixelAspectRatio != AspectRatio(1, 1)) {
			throw Exception("Only 1:1 PAR is supported");
		}

		//Check color model support
		if(videoMode.colorModel != ColorModel::RGB) {
			throw Exception("Unsupported color model");
		}

		//Check color subsampling support
		if(videoMode.colorSubsampling != ColorSubsampling::NONE) {
			throw Exception("Color subsampling is not supported");
		}

		//Check color format support
		auto formats = Graphics::Image::getPlaneDescriptors(
			videoMode.resolution,
			videoMode.colorSubsampling,
			videoMode.colorFormat
		);

		if(formats.size() != 1) {
			throw Exception("Multi planar formats are not supported");
		}

		auto& f = formats[0];
		std::tie(f.format, f.swizzle) = Graphics::optimizeFormat(std::make_tuple(f.format, f.swizzle));
		if(formats[0].swizzle != vk::ComponentMapping()) {
			throw Exception("Swizzled formats are not supported");
		}

		const auto& supportedFormats = vulkan.getFormatSupport().framebuffer; //TODO particularize for this window
		if( !std::binary_search(supportedFormats.cbegin(),
								supportedFormats.cend(),
								f.format )) 
		{
			throw Exception("Unsupported format");
		}

		//Check color range support
		if(videoMode.colorRange != ColorRange::FULL) {
			throw Exception("Only full range color is supported");
		} 

		const auto colorSpace = Graphics::toVulkan(videoMode.colorPrimaries, videoMode.colorTransferFunction);

		//Create the color transfer characteristics
		Graphics::ColorTransfer colorTransfer(
			videoMode.colorFormat,
			videoMode.colorRange,
			videoMode.colorTransferFunction,
			videoMode.colorModel,
			videoMode.colorPrimaries
		);
		colorTransfer.optimize(formats, supportedFormats);

		//Try to open it
		opened = std::make_unique<Impl::Open>(
			vulkan,
			videoMode.resolution,
			name,
			f.format,
			colorSpace,
			colorTransfer
		);
	}

	void close() {
		opened.reset();
	}

	void update() {
		assert(opened);
		printf("Hola\n");
		
		const auto& vulkan = opened->vulkan;

		Graphics::Uploader uplo(
			vulkan,
			Resolution(1280, 720),
			AspectRatio(1, 1),
			ColorSubsampling::NONE,
			ColorFormat::R8G8B8A8,
			ColorRange::FULL,
			ColorTransferFunction::LINEAR,
			ColorModel::RGB,
			ColorPrimaries::BT709
		);
		const auto frame = uplo.acquireFrame();
		const auto& pixels = frame->getPixelData();
		for(size_t i = 0; i < pixels[0].size(); i+=4){
			pixels[0][i + 0] = static_cast<std::byte>(rand());
			pixels[0][i + 1] = static_cast<std::byte>(rand());
			pixels[0][i + 2] = static_cast<std::byte>(rand());
			pixels[0][i + 3] = static_cast<std::byte>(0xff);
		}

		frame->flush();

		//Wait for the previous rendering to be completed
		vulkan.getDevice().waitForFences(	*(opened->renderFinishedFence), 
											true, 
											NO_TIMEOUT, 
											vulkan.getDispatcher() );

		//Acquire an image from the swapchain
		const auto index = vulkan.getDevice().acquireNextImageKHR(
			*(opened->swapchain), 						
			NO_TIMEOUT,
			*(opened->imageAvailableSemaphore),
			nullptr,
			vulkan.getDispatcher()
		);

		//Resize the geometry if needed
		opened->geometry.useFrame(*frame);

		const auto& cmdBuffer = opened->commandBuffer;
		const auto& frameBuffer = *(opened->framebuffers[index.value]);

		//Begin creating the command buffer
		const vk::CommandBufferBeginInfo cmdBegin(
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 
			nullptr
		);
		cmdBuffer.begin(cmdBegin, vulkan.getDispatcher());

		const std::array clearValue = {
			vk::ClearValue(std::array{ 0.0f, 0.0f, 0.0f, 0.0f })
		};

		const vk::RenderPassBeginInfo rendBegin(
			*(opened->renderPass),
			frameBuffer,
			vk::Rect2D({0, 0}, opened->extent),
			clearValue.size(), clearValue.data()
		);

		cmdBuffer.beginRenderPass(rendBegin, vk::SubpassContents::eInline, vulkan.getDispatcher());
		cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *(opened->pipeline), vulkan.getDispatcher());

		opened->geometry.bind(cmdBuffer);


		frame->bind(
			cmdBuffer,
			opened->pipelineLayout,
			0,
			vk::Filter::eNearest
		);
		cmdBuffer.draw(4, 1, 0, 0, vulkan.getDispatcher());

		cmdBuffer.endRenderPass(vulkan.getDispatcher());
		cmdBuffer.end(vulkan.getDispatcher());

		//Send it to the queue
		const std::array imageAvailableSemaphores = {
			*(opened->imageAvailableSemaphore)
		};
		
		const std::array renderFinishedSemaphores = {
			*(opened->renderFinishedSemaphore)
		};

		const std::array commandBuffers = {
			cmdBuffer
		};

		const std::array pipelineStages = {
			vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		};

		const vk::SubmitInfo subInfo(
			imageAvailableSemaphores.size(), imageAvailableSemaphores.data(),	//Wait semaphores
			pipelineStages.data(),												//Pipeline stages
			commandBuffers.size(), commandBuffers.data(),						//Command buffers
			renderFinishedSemaphores.size(), renderFinishedSemaphores.data()	//Signal semaphores
		);

		vulkan.getDevice().resetFences(*(opened->renderFinishedFence), vulkan.getDispatcher());
		vulkan.getGraphicsQueue().submit(subInfo, *(opened->renderFinishedFence), vulkan.getDispatcher());

		//Present it
		vulkan.present(*(opened->swapchain), index.value, renderFinishedSemaphores.front());
	}

private:
	static vk::UniqueCommandPool createCommandPool(const Graphics::Vulkan& vulkan)
	{
		constexpr auto createFlags = 
			vk::CommandPoolCreateFlagBits::eTransient | 		//Re-recorded often
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer;	//Re-recorded individually

		const vk::CommandPoolCreateInfo createInfo(
			createFlags,										//Flags
			vulkan.getGraphicsQueueIndex()						//Queue index
		);

		return vulkan.createCommandPool(createInfo);
	}

	static vk::CommandBuffer createCommandBuffer(	const Graphics::Vulkan& vulkan,
													vk::CommandPool pool )
	{
		const vk::CommandBufferAllocateInfo allocInfo(
			pool,
			vk::CommandBufferLevel::ePrimary,
			1
		);

		//Perform a manual allocation, as there is only need for one. 
		//Dont use a smart handle, as it gets freed automatically with the command pool
		vk::CommandBuffer result;
		vulkan.getDevice().allocateCommandBuffers(&allocInfo, &result, vulkan.getDispatcher());
		return result;
	}

	static vk::UniqueSwapchainKHR createSwapchain(	const Graphics::Vulkan& vulkan, 
													vk::SurfaceKHR surface, 
													vk::Extent2D windowExtent, 
													vk::Format format,
													vk::ColorSpaceKHR colorSpace,
													vk::SwapchainKHR old )
	{
		const auto& physicalDevice = vulkan.getPhysicalDevice();

		if(!physicalDevice.getSurfaceSupportKHR(0, surface, vulkan.getDispatcher())){
			throw Exception("Window surface not suppoted by the physical device");
		}

		const auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface, vulkan.getDispatcher());
		const auto imageCount = getImageCount(capabilities);
		const auto extent = getExtent(capabilities, windowExtent);

		const auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface, vulkan.getDispatcher());
		const auto surfaceFormat = getSurfaceFormat(surfaceFormats, vk::SurfaceFormatKHR(format, colorSpace));

		const auto queueFamilies = getQueueFamilies(vulkan);
		const auto sharingMode = (queueFamilies.size() > 1)
									? vk::SharingMode::eConcurrent
									: vk::SharingMode::eExclusive;
		
		const auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface, vulkan.getDispatcher());
		const auto presentMode = getPresentMode(presentModes);

		const vk::SwapchainCreateInfoKHR createInfo(
			{},													//Flags
			surface,											//Sufrace
			imageCount,											//Image count
			surfaceFormat.format,								//Image format
			surfaceFormat.colorSpace,							//Image color space
			extent,												//Image extent
			1,													//Image layer count
			vk::ImageUsageFlagBits::eColorAttachment,			//Image usage
			sharingMode,										//Sharing
			queueFamilies.size(), queueFamilies.data(),			//Used queue families
			capabilities.currentTransform,						//Transformations
			vk::CompositeAlphaFlagBitsKHR::eOpaque,				//Compositing
			presentMode,										//Present mode
			true,												//Clipping
			old													//Old swapchain
		);

		return vulkan.createSwapchain(createInfo);
	}

	static std::vector<vk::UniqueImageView> createImageViews(	const Graphics::Vulkan& vulkan,
																vk::SwapchainKHR swapchain,
																vk::Format format ) 
	{
		const auto images = vulkan.getDevice().getSwapchainImagesKHR(swapchain, vulkan.getDispatcher());
		std::vector<vk::UniqueImageView> result(images.size());

		for(size_t i = 0; i < images.size(); i++){
			const vk::ImageViewCreateInfo createInfo(
				{},												//Flags
				images[i],										//Image
				vk::ImageViewType::e2D,							//ImageView type
				format,											//Image format
				{},												//Swizzle
				vk::ImageSubresourceRange(						//Image subresources
					vk::ImageAspectFlagBits::eColor,				//Aspect mask
					0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
				)
			);

			result[i] =  vulkan.createImageView(createInfo);
		}

		return result;
	}

	static vk::UniqueRenderPass createRenderPass(	const Graphics::Vulkan& vulkan, 
													vk::Format format )
	{
		const std::array attachments = {
			vk::AttachmentDescription(
				{},												//Flags
				format,											//Attachemnt format
				vk::SampleCountFlagBits::e1,					//Sample count
				vk::AttachmentLoadOp::eClear,					//Color attachment load operation
				vk::AttachmentStoreOp::eStore,					//Color attachemnt store operation
				vk::AttachmentLoadOp::eDontCare,				//Stencil attachment load operation
				vk::AttachmentStoreOp::eDontCare,				//Stencil attachment store operation
				vk::ImageLayout::eUndefined,					//Initial layout
				vk::ImageLayout::ePresentSrcKHR					//Final layout
			)
		};

		const std::array attachmentReferences = {
			vk::AttachmentReference(
				0, 												//Attachments index
				vk::ImageLayout::eColorAttachmentOptimal 		//Attachemnt layout
			)
		};

		const std::array subpasses = {
			vk::SubpassDescription(
				{},												//Flags
				vk::PipelineBindPoint::eGraphics,				//Pipeline bind point
				0, nullptr,										//Input attachments
				attachmentReferences.size(), attachmentReferences.data(), //Color attachments
				nullptr,										//Resolve attachemnts
				nullptr,										//Depth / Stencil attachemnts
				0, nullptr										//Preserve attachments
			)
		};

		const std::array subpassDependencies = {
			vk::SubpassDependency(
				VK_SUBPASS_EXTERNAL,							//Source subpass
				0,												//Destination subpass
				vk::PipelineStageFlagBits::eColorAttachmentOutput,//Source stage
				vk::PipelineStageFlagBits::eColorAttachmentOutput,//Destination stage
				{},												//Source access mask
				vk::AccessFlagBits::eColorAttachmentRead | 		//Destintation access mask
					vk::AccessFlagBits::eColorAttachmentWrite
			)
		};

		const vk::RenderPassCreateInfo createInfo(
			{},													//Flags
			attachments.size(), attachments.data(),				//Attachemnts
			subpasses.size(), subpasses.data(),					//Subpasses
			subpassDependencies.size(), subpassDependencies.data()//Subpass dependencies
		);

		return vulkan.createRenderPass(createInfo);
	}

	static std::vector<vk::UniqueFramebuffer> createFramebuffers(	const Graphics::Vulkan& vulkan,
																	vk::RenderPass renderPass,
																	const std::vector<vk::UniqueImageView>& imageViews,
																	vk::Extent2D extent )
	{
		std::vector<vk::UniqueFramebuffer> result(imageViews.size());

		for(size_t i = 0; i < result.size(); i++){
			const std::array attachments = {
				*imageViews[i]
			};

			const vk::FramebufferCreateInfo createInfo(
				{},
				renderPass,
				attachments.size(),
				attachments.data(),
				extent.width, extent.height,
				1
			);

			result[i] = vulkan.createFramebuffer(createInfo);
		}

		return result;
	}

	static vk::DescriptorSetLayout createDescriptorSetLayout(const Graphics::Vulkan& vulkan) {
		static const Utils::StaticId id;

		auto result = vulkan.createDescriptorSetLayout(id);

		if(!result) {
			//Create the bindings
			const std::array bindings = {
				vk::DescriptorSetLayoutBinding(	//UBO binding
					UBO_BINDING,									//Binding
					vk::DescriptorType::eUniformBuffer,				//Type
					1,												//Count
					vk::ShaderStageFlagBits::eAllGraphics,			//Shader stage
					nullptr											//Inmutable samplers
				), 
			};

			const vk::DescriptorSetLayoutCreateInfo createInfo(
				{},
				bindings.size(), bindings.data()
			);

			result = vulkan.createDescriptorSetLayout(id, createInfo);
		}

		return result;
	}

	static vk::PipelineLayout createPipelineLayout(	const Graphics::Vulkan& vulkan, 
													vk::Filter filter ) 
	{
		static std::array<Utils::StaticId, VK_FILTER_RANGE_SIZE> ids;
		const auto layoutId = ids[static_cast<size_t>(filter)];

		auto result = vulkan.createPipelineLayout(layoutId);

		if(!result) {
			const std::array layouts = {
				Graphics::Frame::getDescriptorSetLayout(vulkan, filter),
				//createDescriptorSetLayout(vulkan)
			};

			const vk::PipelineLayoutCreateInfo createInfo(
				{},													//Flags
				layouts.size(), layouts.data(),						//Descriptor set layouts
				0, nullptr											//Push constants
			);

			result = vulkan.createPipelineLayout(layoutId, createInfo);
		}

		return result;
	}

	static vk::UniquePipeline createPipeline(	const Graphics::Vulkan& vulkan,
												const Graphics::Frame::Geometry& geom,
												vk::RenderPass renderPass,
												vk::PipelineLayout layout,
												vk::Extent2D extent )
	{
		static //So that its ptr can be used as an indentifier
		#include <window_vert.h>
		const size_t vertId = reinterpret_cast<uintptr_t>(window_vert);
		static
		#include <window_frag.h>
		const size_t fragId = reinterpret_cast<uintptr_t>(window_frag);

		//Try to retrive modules from cache
		auto vertexShader = vulkan.createShaderModule(vertId);
		auto fragmentShader = vulkan.createShaderModule(fragId);

		if(!vertexShader || !fragmentShader) {
			//Modules aren't in cache. Create them
			vertexShader = vulkan.createShaderModule(vertId, window_vert);
			fragmentShader = vulkan.createShaderModule(fragId, window_frag);
		}

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

		const std::array vertexBindings = {
			geom.getBindingDescription()
		};

		const std::array vertexAttributes = {
			geom.getAttributeDescriptions(VERTEX_POSITION, VERTEX_TEXCOORD)
		};

		const vk::PipelineVertexInputStateCreateInfo vertexInput(
			{},
			vertexBindings.size(), vertexBindings.data(),		//Vertex bindings
			vertexAttributes.size(), vertexAttributes.data()	//Vertex attributes
		);

		const vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
			{},													//Flags
			vk::PrimitiveTopology::eTriangleStrip,				//Topology
			false												//Restart enable
		);

		const std::array viewports = {
			vk::Viewport(
				0.0f, 0.0f,										//Origin
				static_cast<float>(extent.width), 				//Width
				static_cast<float>(extent.height),				//Height
				0.0f, 1.0f										//min, max depth
			),
		};

		const std::array scissors = {
			vk::Rect2D(
				{ 0, 0 },										//Origin
				extent											//Size
			),
		};

		const vk::PipelineViewportStateCreateInfo viewport(
			{},													//Flags
			viewports.size(), viewports.data(),					//Viewports
			scissors.size(), scissors.data()					//Scissors
		);

		const vk::PipelineRasterizationStateCreateInfo rasterizer(
			{},													//Flags
			false, 												//Depth clamp enabled
			false,												//Rasterizer discard enable
			vk::PolygonMode::eFill,								//Plygon mode
			vk:: CullModeFlagBits::eNone, 						//Cull faces
			vk::FrontFace::eClockwise,							//Front face direction
			false, 0.0f, 0.0f, 0.0f,							//Depth bias
			1.0f												//Line width
		);

		const vk::PipelineMultisampleStateCreateInfo multisample(
			{},													//Flags
			vk::SampleCountFlagBits::e1,						//Sample count
			false, 1.0f,										//Sample shading enable, min sample shading
			nullptr,											//Sample mask
			false, false										//Alpha to coverage, alpha to 1 enable
		);

		const vk::PipelineDepthStencilStateCreateInfo depthStencil(
			{},													//Flags
			false, false, 										//Depth test enable, write
			vk::CompareOp::eNever, 								//Depth compare op
			false,												//Depth bounds test
			false, 												//Stencil enabled
			{}, {},												//Stencil operation stete front, back
			0.0f, 0.0f											//min, max depth bounds
		);

		const std::array colorBlendAttachments = {
			vk::PipelineColorBlendAttachmentState(
				true,											//Enabled
				//Cf' = Ai*Ci + (1.0-Ai)*Cf; Typical color mixing equation
				vk::BlendFactor::eSrcAlpha,						//Source color weight
				vk::BlendFactor::eOneMinusSrcAlpha,				//Destination color weight
				vk::BlendOp::eAdd,								//Color operation
				//Af' = Ai + (1.0-Ai)*Af = Ai + Af - Ai*Af; So that Af' is always greater than Af and Ai
				//https://www.wolframalpha.com/input/?i=plot+%7C+x+%2B+y+-+x+y+%7C+x+%3D+0+to+1+y+%3D+0+to+1
				vk::BlendFactor::eOne,							//Source alpha weight
				vk::BlendFactor::eOneMinusSrcAlpha,				//Destination alpha weight
				vk::BlendOp::eAdd,								//Alpha operation
				vk::ColorComponentFlagBits::eR |				//Color write mask
					vk::ColorComponentFlagBits::eG |
					vk::ColorComponentFlagBits::eB |
					vk::ColorComponentFlagBits::eA
			)
		};

		const vk::PipelineColorBlendStateCreateInfo colorBlend(
			{},													//Flags
			false,												//Enable logic operation
			vk::LogicOp::eCopy,									//Logic operation
			colorBlendAttachments.size(), colorBlendAttachments.data() //Blend attachments
		);

		const vk::PipelineDynamicStateCreateInfo dynamicState(
			{},													//Flags
			0, nullptr											//Dynamis states
		);

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
			layout,												//Pipeline layout
			renderPass, 0,										//Renderpasses
			nullptr, static_cast<uint32_t>(pipelineId)			//Inherit
		);

		return vulkan.createGraphicsPipeline(createInfo);
	}



	static vk::Extent2D getExtent(	const vk::SurfaceCapabilitiesKHR& cap, 
									vk::Extent2D windowExtent )
	{
		constexpr auto INVALID_EXTENT = vk::Extent2D(
			std::numeric_limits<uint32_t>::max(), 
			std::numeric_limits<uint32_t>::max()
		);

		if(cap.currentExtent != INVALID_EXTENT){
			return cap.currentExtent;
		} else {
			return vk::Extent2D(
				std::max(cap.minImageExtent.width, std::min(windowExtent.width, cap.maxImageExtent.width)),
				std::max(cap.minImageExtent.height, std::min(windowExtent.height, cap.maxImageExtent.height))
			);
		}
	}

	static vk::SurfaceFormatKHR getSurfaceFormat(	const std::vector<vk::SurfaceFormatKHR>& formats,
													vk::SurfaceFormatKHR desired )
	{
		if(std::find(formats.cbegin(), formats.cend(), desired) != formats.cend()){
			return desired;
		}

		throw Exception("Unsupported format!");
	}

	static uint32_t getImageCount(const vk::SurfaceCapabilitiesKHR& cap){
		const uint32_t desired = cap.minImageCount + 1;

		if(cap.maxImageCount){
			return std::min(desired, cap.maxImageCount);
		} else {
			return desired;
		}
	}

	static vk::PresentModeKHR getPresentMode(const std::vector<vk::PresentModeKHR>& presentModes){
		const std::array prefered = {
			vk::PresentModeKHR::eMailbox,
			vk::PresentModeKHR::eFifo //Required to be supported.
		};

		for(auto mode : prefered){
			if(std::find(presentModes.cbegin(), presentModes.cend(), mode) != presentModes.cend()){
				return mode; //Found a apropiate one
			}
		}

		throw Exception("No compatible presentation mode was found");
	}

	static std::vector<uint32_t> getQueueFamilies(const Graphics::Vulkan& vulkan){
		const std::set<uint32_t> families = {
			vulkan.getGraphicsQueueIndex(),
			vulkan.getPresentationQueueIndex()
		};

		return std::vector<uint32_t>(families.cbegin(), families.cend());
	}

};

Window::Window(Instance& instance, const std::string& name)
	: ZuazoBase(instance, name, PADS)
	, m_impl()
{
	setUpdateCallback(std::bind(&Impl::update, std::ref(*m_impl)));
}

Window::Window(Instance& instance, std::string&& name)
	: ZuazoBase(instance, std::move(name), PADS)
	, m_impl()
{
	setUpdateCallback(std::bind(&Impl::update, std::ref(*m_impl)));
}

Window::Window(Window&& other) = default;

Window::~Window() {
	if(isOpen()) {
		close();
	}
}

Window& Window::operator=(Window&& other) = default;

void Window::open() {
	const auto& videoMode = getInstance().getApplicationInfo().defaultVideoMode; //TODO

	m_impl->open(getInstance().getVulkan(), videoMode, getName());
	ZuazoBase::enablePeriodicUpdate(Instance::OUTPUT_PRIORITY, 
									Timing::getPeriod(videoMode.frameRate) );
	ZuazoBase::open();
}

void Window::close() {
	ZuazoBase::disablePeriodicUpdate();
	m_impl->close();
	ZuazoBase::close();
}

void Window::setVideoMode(const VideoMode& videoMode) {
	//TODO
	ZUAZO_IGNORE_PARAM(videoMode);
}

}