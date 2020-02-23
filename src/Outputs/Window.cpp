#include <Outputs/Window.h>
#include <Graphics/VulkanConversions.h>
#include <Graphics/VulkanUtils.h>
#include <Graphics/ColorTransfer.h>

#include <Window_frag.h>
#include <Window_vert.h>

#include <cmath>
#include <algorithm>

namespace Zuazo::Outputs {

void Window::open(){
	Implementation impl;

	const auto& videoMode = getVideoMode();
	const auto& vulkan = getInstance().getVulkan();
	const auto[requestedExtent, requestedSurfaceFormat] = getVulkanVideoMode();

	//Create the window and its surface
	impl.window = Graphics::GLFW::Window(videoMode.resolution, getName());
	impl.surface = impl.window.getSurface(vulkan);

	//Evaluate the actual video mode
	const auto surfaceCapabilities = vulkan.getPhysicalDevice().getSurfaceCapabilitiesKHR(*impl.surface, vulkan.getDispatcher());
	const auto surfaceFormats = vulkan.getPhysicalDevice().getSurfaceFormatsKHR(*impl.surface, vulkan.getDispatcher());
	const auto extent = getExtent(surfaceCapabilities, requestedExtent);
	const auto surfaceFormat = getSurfaceFormat(surfaceFormats, requestedSurfaceFormat);

	//Create the swapchain and retrieve images from it
	impl.swapchain = createSwapchain(vulkan, *impl.surface, extent, surfaceFormat, surfaceCapabilities);
	impl.swapchainImageViews = createImageViews(vulkan, *impl.swapchain, surfaceFormat.format);

	//Create the renderpass and pipeline
	impl.renderPass = createRenderPass(vulkan, surfaceFormat.format);
	impl.pipelineLayout = createPipelineLayout(vulkan);
	impl.pipeline = createPipeline(vulkan, *impl.renderPass, *impl.pipelineLayout, extent);
	impl.framebuffers = createFramebuffers(vulkan, *impl.renderPass, impl.swapchainImageViews, extent);

	//Create command pool and command buffers
	impl.commandPool = createCommandPool(vulkan, vulkan.getGraphicsQueueIndex());
	impl.commandBuffers = createCommandBuffers(vulkan, *impl.commandPool, impl.framebuffers.size());

	//Create the semaphores and the fence
	impl.imageAvailableSemaphore = vulkan.createSemaphore();
	impl.renderFinishedSemaphore = vulkan.createSemaphore();
	impl.renderFinishedFence = vulkan.createFence(true);

	//Write the data to the class
	if((requestedExtent != extent) || (requestedSurfaceFormat != surfaceFormat)){
		//Video mode has changed
	}

	m_implementation = std::make_unique<Implementation>(std::move(impl));

	//Setup callbacks
	m_implementation->window.setResolutionCallback([this] (Resolution res){
		auto videoMode = this->getVideoMode();
		videoMode.resolution = res;
		this->ZuazoBase::setVideoMode(videoMode);

		std::lock_guard<std::mutex> lock(this->m_resizeMutex);
		this->recreate();
	});

	ZuazoBase::open();

	drawFrameProvisional();
	vulkan.getDevice().waitIdle(vulkan.getDispatcher());
}

void Window::close(){
	auto& impl = *m_implementation;
	const auto& vulkan = getInstance().getVulkan();

	//Wait until the rendering has finished
	vulkan.getDevice().waitForFences(*impl.renderFinishedFence, true, NO_TIMEOUT, vulkan.getDispatcher());

	m_implementation.reset();
	ZuazoBase::close();
}

void Window::setVideoMode(const VideoMode& videoMode) {
	auto& win = m_implementation->window;

	//Calculate the aproximate size of the window for the desired resolution
	const auto oldRes = static_cast<Math::Vec2i>(win.getResolution());
	const auto oldSize = win.getSize();
	const auto newRes = static_cast<Math::Vec2i>(videoMode.resolution);
	const auto newSize = oldSize * newRes / oldRes;
	win.setSize(newSize);

	ZuazoBase::setVideoMode(videoMode);

	std::lock_guard<std::mutex> lock(m_resizeMutex);
	recreate();
}

std::tuple<vk::Extent2D, vk::SurfaceFormatKHR> Window::getVulkanVideoMode() const{
	const auto& videoMode = getVideoMode();

	const auto extent = Graphics::toVulkan(videoMode.resolution);
	const auto [format, swizzle] = Graphics::optimizeFormat(Graphics::toVulkan(videoMode.colorFormat)[0]);
	const auto colorSpace = Graphics::toVulkan(videoMode.colorPrimaries, videoMode.colorTransferFunction);
	const auto surfaceFormat = vk::SurfaceFormatKHR(format, colorSpace);

	return std::make_tuple(extent, surfaceFormat);
}

void Window::recreate(){
	auto& impl = *m_implementation;
	const auto& vulkan = getInstance().getVulkan();
	const auto[requestedExtent, requestedSurfaceFormat] = getVulkanVideoMode();

	//Wait until the rendering has finished
	vulkan.getDevice().waitForFences(*impl.renderFinishedFence, true, NO_TIMEOUT, vulkan.getDispatcher());

	//Reset all the needed elements
	impl.commandBuffers.clear();
	impl.framebuffers.clear();
	impl.renderPass.reset();
	impl.pipeline.reset();
	impl.swapchainImageViews.clear();

	//Evaluate the actual video mode
	const auto surfaceCapabilities = vulkan.getPhysicalDevice().getSurfaceCapabilitiesKHR(*impl.surface, vulkan.getDispatcher());
	const auto surfaceFormats = vulkan.getPhysicalDevice().getSurfaceFormatsKHR(*impl.surface, vulkan.getDispatcher());
	const auto extent = getExtent(surfaceCapabilities, requestedExtent);
	const auto surfaceFormat = getSurfaceFormat(surfaceFormats, requestedSurfaceFormat);

	//Recreate stuff
	impl.swapchain = createSwapchain(vulkan, *impl.surface, extent, surfaceFormat, surfaceCapabilities, *impl.swapchain);
	impl.swapchainImageViews = createImageViews(vulkan, *impl.swapchain, surfaceFormat.format);
	impl.renderPass = createRenderPass(vulkan, surfaceFormat.format);
	impl.pipeline = createPipeline(vulkan, *impl.renderPass, *impl.pipelineLayout, extent);
	impl.framebuffers = createFramebuffers(vulkan, *impl.renderPass, impl.swapchainImageViews, extent);
	impl.commandBuffers = createCommandBuffers(vulkan, *impl.commandPool, impl.framebuffers.size());
}




void Window::drawFrameProvisional(){
	const auto& vulkan = getInstance().getVulkan();
	const auto& impl = *m_implementation;

	std::lock_guard<std::mutex> lock(m_resizeMutex);

	//Wait for the previous rendering to be completed
	vulkan.getDevice().waitForFences(*impl.renderFinishedFence, true, NO_TIMEOUT, vulkan.getDispatcher());

	//Acquire an image from the swapchain
	const auto index = vulkan.getDevice().acquireNextImageKHR(
		*impl.swapchain, 						
		NO_TIMEOUT,
		*impl.imageAvailableSemaphore,
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
		vk::ClearValue(std::array{ 0.0f, 0.0f, 0.0f, 0.0f })
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
	const std::array imageAvailableSemaphores = {
		*impl.imageAvailableSemaphore
	};
	
	const std::array renderFinishedSemaphores = {
		*impl.renderFinishedSemaphore
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

	vulkan.getDevice().resetFences(*impl.renderFinishedFence, vulkan.getDispatcher());
	vulkan.getGraphicsQueue().submit(subInfo, *impl.renderFinishedFence, vulkan.getDispatcher());

	//Present it
	const std::array swapchains = {
		*impl.swapchain
	};

	const std::array indices = {
		index.value
	};

	const vk::PresentInfoKHR presentInfo(
		renderFinishedSemaphores.size(), renderFinishedSemaphores.data(),	//Wait semaphores
		swapchains.size(), swapchains.data(), 								//Swapchains
		indices.data(),														//Image index
		nullptr																//Results								
	);

	switch(vulkan.getPresentationQueue().presentKHR(&presentInfo, vulkan.getDispatcher())){
	case vk::Result::eSuccess: 
		break;
	case vk::Result::eErrorOutOfDateKHR: 
		recreate();
		return;
	default:
		return;
	}
}


vk::UniqueSwapchainKHR Window::createSwapchain(	const Graphics::Vulkan& vulkan, 
												const vk::SurfaceKHR& surface, 
												const vk::Extent2D& extent, 
												const vk::SurfaceFormatKHR& surfaceFormat,
												const vk::SurfaceCapabilitiesKHR& capabilities,
												vk::SwapchainKHR old )
{
	const auto& physicalDevice = vulkan.getPhysicalDevice();
	const auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface, vulkan.getDispatcher());

	if(!physicalDevice.getSurfaceSupportKHR(0, surface, vulkan.getDispatcher())){
		throw Exception("Window surface not suppoted by the physical device");
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

	return vulkan.createSwapchain(createInfo);
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

		result[i] =  vulkan.createImageView(createInfo);
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

	return vulkan.createRenderPass(createInfo);
}

vk::UniquePipelineLayout Window::createPipelineLayout(const Graphics::Vulkan& vulkan) {
	constexpr vk::Filter filter = vk::Filter::eLinear; //TODO

	std::array descriptors = {
		vulkan.getColorTransferDescriptor(filter)
	};

	const vk::PipelineLayoutCreateInfo createInfo(
		{},													//Flags
		descriptors.size(), descriptors.data(),				//Descriptor set layouts
		0, nullptr											//Push constants
	);

	return vulkan.createPipelineLayout(createInfo);
}

vk::UniquePipeline Window::createPipeline(	const Graphics::Vulkan& vulkan,
											vk::RenderPass renderPass,
											vk::PipelineLayout layout,
											const vk::Extent2D& extent )
{
	const auto vertexShader = vulkan.createShader(Utils::BufferView(Window_vert, sizeof(Window_vert) / 4));
	const auto fragmentShader = vulkan.createShader(Utils::BufferView(Window_frag, sizeof(Window_frag) / 4));

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

	return vulkan.createGraphicsPipeline(nullptr, createInfo);
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

		result[i] = vulkan.createFramebuffer(createInfo);
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

	return vulkan.createCommandPool(createInfo);
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

	return vulkan.allocateCommnadBuffers(allocInfo);
}







vk::Extent2D Window::getExtent(	const vk::SurfaceCapabilitiesKHR& cap, 
								const vk::Extent2D& windowExtent )
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

vk::SurfaceFormatKHR Window::getSurfaceFormat(	const std::vector<vk::SurfaceFormatKHR>& formats,
												const vk::SurfaceFormatKHR& desired )
{
	if(std::find(formats.cbegin(), formats.cend(), desired) != formats.cend()){
		return desired;
	}

	return formats[0]; //TODO fins closest match
}

uint32_t Window::getImageCount(const vk::SurfaceCapabilitiesKHR& cap){
	const uint32_t desired = cap.minImageCount + 1;

	if(cap.maxImageCount){
		return std::min(desired, cap.maxImageCount);
	} else {
		return desired;
	}
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

std::vector<uint32_t> Window::getQueueFamilies(const Graphics::Vulkan& vulkan){
	const std::set<uint32_t> families = {
		vulkan.getGraphicsQueueIndex(),
		vulkan.getPresentationQueueIndex()
	};

	return std::vector<uint32_t>(families.cbegin(), families.cend());
}


}