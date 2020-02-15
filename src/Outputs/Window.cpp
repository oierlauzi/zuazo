#include <Outputs/Window.h>
#include <Graphics/VulkanConversions.h>
#include <Graphics/VulkanUtils.h>

#include <window_frag.h>
#include <window_vert.h>

namespace Zuazo::Outputs {

void Window::open(){
	Implementation impl;

	const auto& videoMode = getVideoMode();
	const auto& vulkan = getInstance().getVulkan();
	const auto extent = Graphics::toVulkan(videoMode.resolution);
	const auto surfaceFormat = Graphics::toVulkan(	videoMode.colorFormat, 
													videoMode.colorPrimaries, 
													videoMode.colorEncoding );

	//Create the window, its surface and swapchain. //TODO get surface format compatibility
	impl.window = Graphics::GLFW::Window(videoMode.resolution, getName());
	impl.surface = impl.window.getSurface(vulkan);
	impl.swapchain = createSwapchain(vulkan, *impl.surface, extent, surfaceFormat);
	impl.swapchainImageViews = createImageViews(vulkan, *impl.swapchain, surfaceFormat.format);

	//Create the renderpass and pipeline
	impl.renderPass = createRenderPass(vulkan, surfaceFormat.format);
	impl.pipelineLayout = createPipelineLayout(vulkan);
	impl.pipeline = createPipeline(vulkan, *impl.renderPass, *impl.pipelineLayout, extent);
	impl.framebuffers = createFramebuffers(vulkan, *impl.renderPass, impl.swapchainImageViews, extent);

	//Create command pool and command buffers
	impl.commandPool = createCommandPool(vulkan, vulkan.getGraphicsQueueIndex());
	impl.commandBuffers = createCommandBuffers(vulkan, *impl.commandPool, impl.framebuffers.size());

	//Create the semaphores
	impl.imageAvailable = createSemaphore(vulkan);
	impl.renderFinished = createSemaphore(vulkan);

	m_implementation = std::make_unique<Implementation>(std::move(impl));
	ZuazoBase::open();

	while(true){
		drawFrameProvisional();
	}
}

void Window::close(){
	m_implementation.reset();
	ZuazoBase::close();
}

void Window::setVideoMode(const VideoMode& videoMode) {
	//TODO
	ZuazoBase::setVideoMode(videoMode);
}

void Window::drawFrameProvisional(){
	const auto& vulkan = getInstance().getVulkan();
	const auto& impl = *m_implementation;

	const auto index = vulkan.getDevice().acquireNextImageKHR(
		*impl.swapchain, 						
		std::numeric_limits<uint64_t>::max(),
		*impl.imageAvailable,
		nullptr,
		vulkan.getDispatcher()
	);

	const auto& cmdBuffer = *(impl.commandBuffers[index.value]);
    const auto& frameBuffer = *(impl.framebuffers[index.value]);

	//Begin creating the command buffer
    const vk::CommandBufferBeginInfo cmdBegin(
		vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 
		nullptr
	);
    cmdBuffer.begin(cmdBegin, vulkan.getDispatcher());

	const std::array clearValue = {
		vk::ClearValue(std::array{ 0.0f, 0.0f, 0.0f, 1.0f })
	};

	vk::RenderPassBeginInfo rendBegin(
		*impl.renderPass,
		frameBuffer,
		vk::Rect2D({0, 0}, Zuazo::Graphics::toVulkan(getVideoMode().resolution)),
		clearValue.size(), clearValue.data()
	);

	cmdBuffer.beginRenderPass(rendBegin, vk::SubpassContents::eInline, vulkan.getDispatcher());
	cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *impl.pipeline, vulkan.getDispatcher());

	cmdBuffer.draw(4, 1, 0, 0, vulkan.getDispatcher());

	cmdBuffer.endRenderPass(vulkan.getDispatcher());
	cmdBuffer.end(vulkan.getDispatcher());

	//Send it to the queue
	const std::array waitSemaphores = {
		*impl.imageAvailable
	};
	
	const std::array signalSemaphores = {
		*impl.renderFinished
	};

	const std::array commandBuffers = {
		cmdBuffer
	};

	const std::array pipelineStages = {
		vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput)
	};

	const vk::SubmitInfo subInfo(
		waitSemaphores.size(), waitSemaphores.data(),
		pipelineStages.data(),
		commandBuffers.size(), commandBuffers.data(),
		signalSemaphores.size(), signalSemaphores.data()
	);

	vulkan.getGraphicsQueue().submit(subInfo, nullptr, vulkan.getDispatcher());

	//Present it
	const std::array swapchains = {
		*impl.swapchain
	};

	const std::array indices = {
		index.value
	};

	const vk::PresentInfoKHR presentInfo(
		signalSemaphores.size(), signalSemaphores.data(),
		swapchains.size(), swapchains.data(), 
		indices.data(), nullptr
	);

	vulkan.getPresentationQueue().presentKHR(presentInfo, vulkan.getDispatcher());
	vulkan.getPresentationQueue().waitIdle(vulkan.getDispatcher());
}


vk::UniqueSwapchainKHR Window::createSwapchain(	const Graphics::Vulkan& vulkan, 
												const vk::SurfaceKHR& surface, 
												const vk::Extent2D& extent, 
												const vk::SurfaceFormatKHR& surfaceFormat,
												vk::SwapchainKHR old )
{
	const auto& physicalDevice = vulkan.getPhysicalDevice();
	const auto formats = physicalDevice.getSurfaceFormatsKHR(surface, vulkan.getDispatcher());
	const auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface, vulkan.getDispatcher());
	const auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface, vulkan.getDispatcher());

	if(!physicalDevice.getSurfaceSupportKHR(0, surface, vulkan.getDispatcher())){
		throw Exception("Window surface not suppoted by the physical device");
	}

	if(std::find(formats.cbegin(), formats.cend(), surfaceFormat) == formats.cend()){
		throw Exception("Requested window surface format is not supported");
	}

	if(	extent.width < capabilities.minImageExtent.width || 
		extent.height < capabilities.minImageExtent.height || 
		extent.width > capabilities.maxImageExtent.width || 
		extent.height > capabilities.maxImageExtent.height )
	{
		throw Exception("Requested surface extent is not supported");
	}

	const auto imageCount = getImageCount(capabilities);
	const auto queueFamilies = getQueueFamilies(vulkan);
	const auto sharingMode = (queueFamilies.size() > 1)
								? vk::SharingMode::eConcurrent
								: vk::SharingMode::eExclusive;
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

	return vulkan.getDevice().createSwapchainKHRUnique(createInfo, nullptr, vulkan.getDispatcher());
}

std::vector<vk::UniqueImageView> Window::createImageViews(	const Graphics::Vulkan& vulkan,
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

		result[i] =  vulkan.getDevice().createImageViewUnique(createInfo, nullptr, vulkan.getDispatcher());
	}

	return result;
}

vk::UniqueRenderPass Window::createRenderPass(	const Graphics::Vulkan& vulkan, 
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

	return vulkan.getDevice().createRenderPassUnique(
		createInfo, nullptr,
		vulkan.getDispatcher()
	);
}

vk::UniquePipelineLayout Window::createPipelineLayout(const Graphics::Vulkan& vulkan) {
	const vk::PipelineLayoutCreateInfo createInfo(
		{},													//Flags
		0, nullptr,											//Set layout
		0, nullptr											//Push constants
	);

	return vulkan.getDevice().createPipelineLayoutUnique(
		createInfo, nullptr, 
		vulkan.getDispatcher()
	);
}

vk::UniquePipeline Window::createPipeline(	const Graphics::Vulkan& vulkan,
											vk::RenderPass renderPass,
											vk::PipelineLayout layout,
											const vk::Extent2D& extent )
{
	const auto vertexShader = createShader(vulkan, Utils::BufferView(window_vert, sizeof(window_vert) / 4));
	const auto fragmentShader = createShader(vulkan, Utils::BufferView(window_frag, sizeof(window_frag) / 4));

	constexpr auto SHADER_ENTRY_POINT = "main";
	const std::array shaderStages = {
		vk::PipelineShaderStageCreateInfo(		
			{},												//Flags
			vk::ShaderStageFlagBits::eVertex,				//Shader type
			*vertexShader,									//Shader handle
			SHADER_ENTRY_POINT ),							//Shader entry point
		vk::PipelineShaderStageCreateInfo(		
			{},												//Flags
			vk::ShaderStageFlagBits::eFragment,				//Shader type
			*fragmentShader,								//Shader handle
			SHADER_ENTRY_POINT ),							//Shader entry point
	};

	const vk::PipelineVertexInputStateCreateInfo vertexInput(
		{},
		0, nullptr,											//Vertex bindings
		0, nullptr											//Vertex attributes
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
			false,											//Enabled
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
		nullptr, -1											//Inherit //TODO
	);

	return vulkan.getDevice().createGraphicsPipelineUnique(
		nullptr, createInfo, nullptr,
		vulkan.getDispatcher()
	);
}

std::vector<vk::UniqueFramebuffer> Window::createFramebuffers(	const Graphics::Vulkan& vulkan,
																vk::RenderPass renderPass,
																const std::vector<vk::UniqueImageView>& imageViews,
																const vk::Extent2D& extent )
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

		result[i] = vulkan.getDevice().createFramebufferUnique(createInfo, nullptr, vulkan.getDispatcher());
	}

	return result;
}

vk::UniqueCommandPool Window::createCommandPool(const Graphics::Vulkan& vulkan,
												uint32_t queueIndex )
{
	constexpr auto createFlags = 
		vk::CommandPoolCreateFlagBits::eTransient | 		//Re-recorded often
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer;	//Re-recorded individually

	const vk::CommandPoolCreateInfo createInfo(
		createFlags,										//Flags
		queueIndex											//Queue index
	);

	return vulkan.getDevice().createCommandPoolUnique(createInfo, nullptr, vulkan.getDispatcher());
}

std::vector<vk::UniqueCommandBuffer> Window::createCommandBuffers(	const Graphics::Vulkan& vulkan,
																	vk::CommandPool pool,
																	uint32_t count )
{
	const vk::CommandBufferAllocateInfo allocInfo(
		pool,
		vk::CommandBufferLevel::ePrimary,
		count
	);

	return vulkan.getDevice().allocateCommandBuffersUnique(allocInfo, vulkan.getDispatcher());
}









vk::PresentModeKHR Window::getPresentMode(const std::vector<vk::PresentModeKHR>& presentModes){
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

uint32_t Window::getImageCount(const vk::SurfaceCapabilitiesKHR& cap){
	const uint32_t desired = cap.minImageCount + 1;

	if(cap.maxImageCount){
		return std::min(desired, cap.maxImageCount);
	} else {
		return desired;
	}
}

std::vector<uint32_t> Window::getQueueFamilies(const Graphics::Vulkan& vulkan){
	const std::set<uint32_t> families = {
		vulkan.getGraphicsQueueIndex(),
		vulkan.getPresentationQueueIndex()
	};

	return std::vector<uint32_t>(families.cbegin(), families.cend());
}


}