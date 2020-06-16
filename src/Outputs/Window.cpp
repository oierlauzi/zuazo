#include <Outputs/Window.h>

#include "../Graphics/VulkanUtils.h"

#include <Graphics/GLFW.h>
#include <Graphics/Vulkan.h>
#include <Graphics/VulkanConversions.h>
#include <Graphics/ColorTransfer.h>
#include <Graphics/StagedBuffer.h>
#include <Utils/StaticId.h>
#include <Utils/Functions.h>

#include <cmath>
#include <algorithm>
#include <limits>
#include <set>
#include <mutex>
#include <cassert>
#include <bitset>

namespace Zuazo::Outputs {

/*
 * Window::Impl
 */
struct Window::Impl {
	enum DescriptorLayouts {
		DESCRIPTOR_LAYOUT_WINDOW,
		DESCRIPTOR_LAYOUT_FRAME,

		DESCRIPTOR_LAYOUT_COUNT
	};

	enum WindowDescriptors {
		WINDOW_DESCRIPTOR_VIEWPORT,
		WINDOW_DESCRIPTOR_COLOR_TRANSFER,

		WINDOW_DESCRIPTOR_COUNT
	};

	struct Vertex {
		Math::Vec2f position;
		Math::Vec2f texCoord;
	};

	static constexpr uint32_t VERTEX_BUFFER_BINDING = 0;
	static constexpr uint32_t VERTEX_POSITION = 0;
	static constexpr uint32_t VERTEX_TEXCOORD = 1;

	static constexpr size_t COLOR_TRANSFER_UNIFORM_OFFSET = 0;
	static inline const size_t COLOR_TRANSFER_UNIFORM_SIZE = Graphics::ColorTransfer::size();
	static inline const size_t VIEWPORT_UNIFORM_OFFSET = Utils::align(COLOR_TRANSFER_UNIFORM_OFFSET + COLOR_TRANSFER_UNIFORM_SIZE, 0x100); //256 is the maximum
	static constexpr size_t VIEWPORT_UNIFORM_SIZE = sizeof(glm::vec2);

	static inline const size_t UNIFORM_BUFFER_SIZE = VIEWPORT_UNIFORM_OFFSET + VIEWPORT_UNIFORM_SIZE;


	struct Open {
		Graphics::GLFW::Window						window;
		vk::UniqueSurfaceKHR						surface;
		vk::UniqueCommandPool						commandPool;
		vk::CommandBuffer							commandBuffer;
		Graphics::StagedBuffer						vertexBuffer;
		Graphics::StagedBuffer						uniformBuffer;
		vk::UniqueDescriptorPool					descriptorPool;
		vk::DescriptorSet							descriptorSet;
		vk::UniqueSemaphore 						imageAvailableSemaphore;
		vk::UniqueSemaphore							renderFinishedSemaphore;
		vk::UniqueFence								renderFinishedFence;

		vk::Extent2D								extent;
		vk::Format 									format;
		vk::ColorSpaceKHR 							colorSpace;
		Graphics::ColorTransfer						colorTransfer;
		vk::Filter									filter;
		Graphics::Frame::Geometry					geometry;

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
				Graphics::ColorTransfer&& colorTransfer,
				vk::Filter scalingFilter,
				ScalingMode scalingMode ) 
			: window(size, name)
			, surface(window.getSurface(vulkan))
			, commandPool(createCommandPool(vulkan))
			, commandBuffer(createCommandBuffer(vulkan, *commandPool))
			, vertexBuffer(createVertexBuffer(vulkan))
			, uniformBuffer(createUniformBuffer(vulkan))
			, descriptorPool(createDescriptorPool(vulkan))
			, descriptorSet(createDescriptorSet(vulkan, *descriptorPool))
			, imageAvailableSemaphore(vulkan.createSemaphore())
			, renderFinishedSemaphore(vulkan.createSemaphore())
			, renderFinishedFence(vulkan.createFence(true))

			, extent(Graphics::toVulkan(window.getResolution()))
			, format(format)
			, colorSpace(colorSpace)
			, colorTransfer(std::move(colorTransfer))
			, filter(scalingFilter)
			, geometry(createGeometry(vertexBuffer.data(), scalingMode, Math::Vec2f(extent.width, extent.height)))

			, swapchain(createSwapchain(vulkan, *surface, extent, format, colorSpace, {}))
			, swapchainImageViews(createImageViews(vulkan, *swapchain, format))
			, renderPass(createRenderPass(vulkan, format))
			, framebuffers(createFramebuffers(vulkan, *renderPass, swapchainImageViews, extent))
			, pipelineLayout(createPipelineLayout(vulkan, filter))
			, pipeline(createPipeline(vulkan, *renderPass, pipelineLayout, extent))

		{
			writeDescriptorSets(vulkan);
			updateViewportUniform(vulkan);
			updateColorTransferUniform(vulkan);
		}

		~Open() = default;

		void recreateSwapchain(const Graphics::Vulkan& vulkan) {
			swapchain = createSwapchain(vulkan, *surface, extent, format, colorSpace, *swapchain);
			swapchainImageViews = createImageViews(vulkan, *swapchain, format);
			renderPass = createRenderPass(vulkan, format);
			framebuffers = createFramebuffers(vulkan, *renderPass, swapchainImageViews, extent);
			pipeline = createPipeline(vulkan, *renderPass, pipelineLayout, extent);
		}

		void recreatePipelineLayout(const Graphics::Vulkan& vulkan) {
			pipelineLayout = createPipelineLayout(vulkan, filter);
			pipeline = createPipeline(vulkan, *renderPass, pipelineLayout, extent);
		}

		void updateViewportUniform(const Graphics::Vulkan& vulkan) {
			uniformBuffer.flushData(
				vulkan,
				VIEWPORT_UNIFORM_OFFSET,
				VIEWPORT_UNIFORM_SIZE,
				vulkan.getGraphicsQueueIndex(),
				vk::PipelineStageFlagBits::eVertexShader
			);
		}

		void updateColorTransferUniform(const Graphics::Vulkan& vulkan) {
			uniformBuffer.flushData(
				vulkan,
				COLOR_TRANSFER_UNIFORM_OFFSET,
				COLOR_TRANSFER_UNIFORM_SIZE,
				vulkan.getGraphicsQueueIndex(),
				vk::PipelineStageFlagBits::eFragmentShader
			);
		}

		void waitCompletion(const Graphics::Vulkan& vulkan) {
			vulkan.waitForFences(*renderFinishedFence);
		}
	
	private:
		void writeDescriptorSets(const Graphics::Vulkan& vulkan) {
			const std::array viewportBuffers = {
				vk::DescriptorBufferInfo(
					uniformBuffer.getBuffer(),								//Buffer
					VIEWPORT_UNIFORM_OFFSET,								//Offset
					VIEWPORT_UNIFORM_SIZE									//Size
				)
			};
			const std::array colorTransferBuffers = {
				vk::DescriptorBufferInfo(
					uniformBuffer.getBuffer(),								//Buffer
					COLOR_TRANSFER_UNIFORM_OFFSET,							//Offset
					COLOR_TRANSFER_UNIFORM_SIZE								//Size
				)
			};

			const std::array writeDescriptorSets ={
				vk::WriteDescriptorSet( //Viewport UBO
					descriptorSet,											//Descriptor set
					WINDOW_DESCRIPTOR_VIEWPORT,								//Binding
					0, 														//Index
					viewportBuffers.size(),									//Descriptor count		
					vk::DescriptorType::eUniformBuffer,						//Descriptor type
					nullptr, 												//Images 
					viewportBuffers.data(), 								//Buffers
					nullptr													//Texel buffers
				),
				vk::WriteDescriptorSet( //ColorTransfer UBO
					descriptorSet,											//Descriptor set
					WINDOW_DESCRIPTOR_COLOR_TRANSFER,						//Binding
					0, 														//Index
					colorTransferBuffers.size(),							//Descriptor count		
					vk::DescriptorType::eUniformBuffer,						//Descriptor type
					nullptr, 												//Images 
					colorTransferBuffers.data(), 							//Buffers
					nullptr													//Texel buffers
				)
			};

			vulkan.getDevice().updateDescriptorSets(writeDescriptorSets, {}, vulkan.getDispatcher());
		}
	};

	Instance&									instance;
	const Signal::Input<Video>&					videoIn;

	ScalingMode									scalingMode;
	ScalingFilter								scalingFilter;
	
	std::unique_ptr<Open>						opened;

	Impl(	Instance& instance,
			const Signal::Input<Video>& videoIn )
		: instance(instance)
		, videoIn(videoIn)
		, scalingMode(ScalingMode::BOXED)
		, scalingFilter(ScalingFilter::NEAREST)
	{
	}
	~Impl() = default;

	void open(	const VideoMode& videoMode, 
				std::string_view name ) 
	{
		const auto& vulkan = instance.getVulkan();

		auto [size, format, colorSpace, colorTransfer] = convertParameters(vulkan, videoMode);
		const auto filter = Graphics::toVulkan(scalingFilter);

		//Try to open it
		opened = std::make_unique<Impl::Open>(
			vulkan,
			size,
			name,
			format,
			colorSpace,
			std::move(colorTransfer),
			filter,
			scalingMode
		);

		//Set the callback
		opened->window.setResolutionCallback(std::bind(&Impl::resizeCallback, std::ref(*this), std::placeholders::_1));
	}

	void close() {
		opened->waitCompletion(instance.getVulkan());
		opened.reset();
	}

	void setVideoMode(const VideoMode& videoMode) {
		if(opened) {
			enum {
				RECREATE_SWAPCHAIN,
				UPDATE_COLOR_TRANSFER,

				MODIFICATION_COUNT
			};

			const auto& vulkan = instance.getVulkan();

			std::bitset<MODIFICATION_COUNT> modifications;


			auto [size, format, colorSpace, colorTransfer] = convertParameters(vulkan, videoMode);

			if(opened->window.getSize() != size) {
				//Resizing is required. 
				opened->window.setSize(size);

				opened->extent = Graphics::toVulkan(opened->window.getResolution());
				opened->geometry.setTargetSize(Math::Vec2f(opened->extent.width, opened->extent.height));

				modifications.set(RECREATE_SWAPCHAIN);
			}

			if(opened->format != format) {
				//Format has changed
				opened->format = format;

				modifications.set(RECREATE_SWAPCHAIN);
			}

			if(opened->colorSpace != colorSpace) {
				//Color space has changed
				opened->colorSpace = colorSpace;

				modifications.set(RECREATE_SWAPCHAIN);
			}

			if(opened->colorTransfer != colorTransfer) {
				//Color transfer characteristics have changed
				opened->colorTransfer = std::move(colorTransfer);

				modifications.set(UPDATE_COLOR_TRANSFER);
			}

			//Recreate stuff accordingly
			if(modifications.any()) {
				//Wait until rendering finishes
				opened->waitCompletion(vulkan);

				if(modifications.test(RECREATE_SWAPCHAIN)) opened->recreateSwapchain(vulkan);
				if(modifications.test(UPDATE_COLOR_TRANSFER)) opened->updateColorTransferUniform(vulkan);
			}
		}
	}
	
	void setScalingMode(ScalingMode mode) {
		if(scalingMode != mode) {
			scalingMode = mode;
			
			if(opened) {
				const auto& vulkan = instance.getVulkan();

				opened->waitCompletion(vulkan);
				opened->geometry.setScalingMode(mode);
			}
		}
	}

	ScalingMode getScalingMode() const {
		return scalingMode;
	}


	void setScalingFilter(ScalingFilter filter) {
		if(scalingFilter != filter) {
			scalingFilter = filter;
			
			if(opened) {
				const auto& vulkan = instance.getVulkan();

				opened->waitCompletion(vulkan);
				opened->filter = Graphics::toVulkan(filter);
				opened->recreatePipelineLayout(vulkan);
			}
		}
	}
	
	ScalingFilter getScalingFilter() const {
		return scalingFilter;
	}


	void update() {
		assert(opened);

		if(videoIn.hasChanged()) {
			const auto& vulkan = instance.getVulkan();

			//Input has changed, pull a frame from it
			const auto& frame = videoIn.pull();

			//Wait for the previous rendering to be completed
			opened->waitCompletion(vulkan);

			//Acquire an image from the swapchain
			const auto index = vulkan.getDevice().acquireNextImageKHR(
				*(opened->swapchain), 						
				std::numeric_limits<uint64_t>::max(),
				*(opened->imageAvailableSemaphore),
				nullptr,
				vulkan.getDispatcher()
			);

			//Resize the geometry if needed
			if(frame) {
				opened->vertexBuffer.waitCompletion(vulkan);
				if(opened->geometry.useFrame(*frame)){
					opened->vertexBuffer.flushData(
						vulkan, 
						vulkan.getGraphicsQueueIndex(), 
						vk::PipelineStageFlagBits::eVertexInput
					);
				}	
			}

			const auto& cmdBuffer = opened->commandBuffer;
			const auto& frameBuffer = *(opened->framebuffers[index.value]);


			//Begin writing to the command buffer. //TODO maybe reset pool?
			constexpr vk::CommandBufferBeginInfo cmdBegin(
				vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 
				nullptr
			);
			vulkan.begin(cmdBuffer, cmdBegin);

			//Begin a render pass
			const std::array clearValue = {
				vk::ClearValue(std::array{ 0.0f, 0.0f, 0.0f, 0.0f })
			};
			const vk::RenderPassBeginInfo rendBegin(
				*(opened->renderPass),												//Renderpass
				frameBuffer,														//Target framebuffer
				vk::Rect2D({0, 0}, opened->extent),									//Extent
				clearValue.size(), clearValue.data()								//Attachment clear values
			);
			cmdBuffer.beginRenderPass(rendBegin, vk::SubpassContents::eInline, vulkan.getDispatcher());


			//If it is a valid frame, draw it.
			if(frame) {
				cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *(opened->pipeline), vulkan.getDispatcher());

				cmdBuffer.bindVertexBuffers(
					VERTEX_BUFFER_BINDING,											//Binding
					opened->vertexBuffer.getBuffer(),								//Vertex buffers
					0UL,															//Offsets
					vulkan.getDispatcher()											//Dispatcher
				);

				cmdBuffer.bindDescriptorSets(
					vk::PipelineBindPoint::eGraphics,								//Pipeline bind point
					opened->pipelineLayout,											//Pipeline layout
					DESCRIPTOR_LAYOUT_WINDOW,										//First index
					opened->descriptorSet,											//Descriptor sets
					{},																//Dynamic offsets
					vulkan.getDispatcher()											//Dispatcher
				);

				frame->bind(cmdBuffer, opened->pipelineLayout, DESCRIPTOR_LAYOUT_FRAME, opened->filter);

				cmdBuffer.draw(4, 1, 0, 0, vulkan.getDispatcher());
			}

			//End everything
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
			vulkan.resetFences(*(opened->renderFinishedFence));
			vulkan.submit(vulkan.getGraphicsQueue(), subInfo, *(opened->renderFinishedFence));

			//Present it
			vulkan.present(*(opened->swapchain), index.value, renderFinishedSemaphores.front());
		}
	}

	VideoMode::Compatibilities getVideoModeCompatibility() const {
		const VideoMode::Range<Resolution> resolutions {
			Resolution(1, 1),
			instance.getResolutionSupport().maxOutputResolution
		};

		const VideoMode::MustBe<AspectRatio> aspectRatios {
			AspectRatio(1, 1)
		};

		const VideoMode::Any<Timing::Rate> frameRates {};

		const VideoMode::MustBe<ColorModel> colorModels {
			ColorModel::RGB
		};

		const VideoMode::MustBe<ColorSubsampling> colorSubsamplings {
			ColorSubsampling::NONE
		};

		const VideoMode::MustBe<ColorRange> colorRanges {
			ColorRange::FULL
		};

		//If it is not opened return a generic comatibility
		//Otherwise query it
		if(opened) {
			VideoMode::Compatibilities result;

			const auto& vulkan = instance.getVulkan();
			const auto surfaceFormats = vulkan.getPhysicalDevice().getSurfaceFormatsKHR(*(opened->surface), vulkan.getDispatcher());

			for(const auto& surfaceFormat : surfaceFormats) {
				const auto [colorPrimary, colorTransferFunction] = Graphics::fromVulkan(surfaceFormat.colorSpace);
				const auto [format, colorTransferFunction2] = Graphics::fromVulkan(surfaceFormat.format);

				//Evaluate if it is a valid option
				if(	(colorPrimary != ColorPrimaries::NONE) &&
					(colorTransferFunction != ColorTransferFunction::NONE) &&
					(format != ColorFormat::NONE) )
				{
					const VideoMode::MustBe<ColorPrimaries> colorPrimaries {
						colorPrimary
					};

					const VideoMode::MustBe<ColorTransferFunction> colorTransferFunctions {
						colorTransferFunction
					};

					const VideoMode::MustBe<ColorFormat> colorFormats {
						format
					};

					result.emplace_back(
						resolutions,
						aspectRatios,
						frameRates,
						colorPrimaries,
						colorModels,
						colorTransferFunctions,
						colorSubsamplings,
						colorRanges,
						colorFormats
					);
				}
			}

			return result;
		} else {
			//Return a configuration that is always supported, according to 
			//https://vulkan.gpuinfo.org/listsurfaceformats.php

			const VideoMode::MustBe<ColorPrimaries> colorPrimaries {
				ColorPrimaries::BT709
			};

			const VideoMode::MustBe<ColorTransferFunction> colorTransferFunctions {
				ColorTransferFunction::IEC61966_2_1
			};

			const VideoMode::MustBe<ColorFormat> colorFormats {
				#ifdef __ANDROID__
					ColorFormat::R8G8B8A8
				#else
					ColorFormat::B8G8R8A8
				#endif
			};

			return { 
				VideoMode::Compatibility(
					resolutions,
					aspectRatios,
					frameRates,
					colorPrimaries,
					colorModels,
					colorTransferFunctions,
					colorSubsamplings,
					colorRanges,
					colorFormats
				)
			};
		}
	}

private:
	void resizeCallback(Resolution res) {
		const auto& vulkan = instance.getVulkan();
		std::lock_guard<Instance> lock(instance);

		opened->waitCompletion(vulkan);
		opened->extent = Graphics::toVulkan(res);
		opened->geometry.setTargetSize(Math::Vec2f(res.x, res.y));
		opened->updateViewportUniform(vulkan);
		opened->recreateSwapchain(vulkan);
	}

	static std::tuple<Math::Vec2i, vk::Format, vk::ColorSpaceKHR, Graphics::ColorTransfer>
	convertParameters(	const Graphics::Vulkan& vulkan,
						const VideoMode& videoMode )
	{
		const auto size = static_cast<Math::Vec2i>(
			videoMode.get<VideoMode::Parameters::resolution>()
		);

		//Obatin the pixel format
		auto formats = Graphics::Frame::getPlaneDescriptors(
			videoMode.get<VideoMode::Parameters::resolution>(),
			videoMode.get<VideoMode::Parameters::colorSubsampling>(),
			videoMode.get<VideoMode::Parameters::colorFormat>()
		);
		assert(formats.size() == 1);

		auto& f = formats[0];
		std::tie(f.format, f.swizzle) = Graphics::optimizeFormat(std::make_tuple(f.format, f.swizzle));
		assert(formats[0].swizzle == vk::ComponentMapping());

		//Obtain the color space
		const auto colorSpace = Graphics::toVulkan(
			videoMode.get<VideoMode::Parameters::colorPrimaries>(), 
			videoMode.get<VideoMode::Parameters::colorTransferFunction>()
		);

		//Create the color transfer characteristics
		Graphics::ColorTransfer colorTransfer(
			videoMode.get<VideoMode::Parameters::colorFormat>(),
			videoMode.get<VideoMode::Parameters::colorRange>(),
			videoMode.get<VideoMode::Parameters::colorTransferFunction>(),
			videoMode.get<VideoMode::Parameters::colorModel>(),
			videoMode.get<VideoMode::Parameters::colorPrimaries>()
		);

		const auto& supportedFormats = vulkan.getFormatSupport().framebuffer;
		colorTransfer.optimize(formats, supportedFormats);

		return std::make_tuple(size, f.format, colorSpace, std::move(colorTransfer));
	}




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


	static Graphics::StagedBuffer createVertexBuffer(const Graphics::Vulkan& vulkan) {
		return Graphics::StagedBuffer(
			vulkan,
			sizeof(Vertex) * Graphics::Frame::Geometry::VERTEX_COUNT,
			vk::BufferUsageFlagBits::eVertexBuffer
		);
	}

	static Graphics::StagedBuffer createUniformBuffer(const Graphics::Vulkan& vulkan) {
		return Graphics::StagedBuffer(
			vulkan,
			UNIFORM_BUFFER_SIZE,
			vk::BufferUsageFlagBits::eUniformBuffer
		);
	}

	static vk::DescriptorSetLayout createDescriptorSetLayout(const Graphics::Vulkan& vulkan) {
		static const Utils::StaticId id;

		auto result = vulkan.createDescriptorSetLayout(id);

		if(!result) {
			//Create the bindings
			const std::array bindings = {
				vk::DescriptorSetLayoutBinding(	//UBO binding
					WINDOW_DESCRIPTOR_VIEWPORT,						//Binding
					vk::DescriptorType::eUniformBuffer,				//Type
					1,												//Count
					vk::ShaderStageFlagBits::eVertex,				//Shader stage
					nullptr											//Inmutable samplers
				), 
				vk::DescriptorSetLayoutBinding(	//UBO binding
					WINDOW_DESCRIPTOR_COLOR_TRANSFER,				//Binding
					vk::DescriptorType::eUniformBuffer,				//Type
					1,												//Count
					vk::ShaderStageFlagBits::eFragment,				//Shader stage
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

	static vk::UniqueDescriptorPool createDescriptorPool(const Graphics::Vulkan& vulkan){
		const std::array poolSizes = {
			vk::DescriptorPoolSize(
				vk::DescriptorType::eUniformBuffer,					//Descriptor type
				WINDOW_DESCRIPTOR_COUNT								//Descriptor count
			)
		};

		const vk::DescriptorPoolCreateInfo createInfo(
			{},														//Flags
			1,														//Descriptor set count
			poolSizes.size(), poolSizes.data()						//Pool sizes
		);

		return vulkan.createDescriptorPool(createInfo);
	}

	static vk::DescriptorSet createDescriptorSet(	const Graphics::Vulkan& vulkan,
													vk::DescriptorPool pool )
	{
		const std::array layouts {
			createDescriptorSetLayout(vulkan)
		};

		const vk::DescriptorSetAllocateInfo allocInfo(
			pool,													//Pool
			layouts.size(), layouts.data()							//Layouts
		);

		//Allocate it
		vk::DescriptorSet descriptorSet;
		static_assert(layouts.size() == 1);
		const auto result = vulkan.getDevice().allocateDescriptorSets(&allocInfo, &descriptorSet, vulkan.getDispatcher());

		if(result != vk::Result::eSuccess){
			throw Exception("Error allocating descriptor sets");
		}

		return descriptorSet;
	}

	static Graphics::Frame::Geometry createGeometry(std::byte* data,
													ScalingMode scalingMode,
													Math::Vec2f size )
	{
		return Graphics::Frame::Geometry(
			data, sizeof(Vertex), offsetof(Vertex, position), offsetof(Vertex, texCoord),
			scalingMode, size
		);
	}


	static vk::UniqueSwapchainKHR createSwapchain(	const Graphics::Vulkan& vulkan, 
													vk::SurfaceKHR surface, 
													vk::Extent2D& extent, 
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
		extent = getExtent(capabilities, extent);

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

	static vk::PipelineLayout createPipelineLayout(	const Graphics::Vulkan& vulkan, 
													vk::Filter filter ) 
	{
		static std::array<Utils::StaticId, VK_FILTER_RANGE_SIZE> ids;
		const auto layoutId = ids[static_cast<size_t>(filter)];

		auto result = vulkan.createPipelineLayout(layoutId);

		if(!result) {
			const std::array layouts = {
				createDescriptorSetLayout(vulkan),
				Graphics::Frame::getDescriptorSetLayout(vulkan, filter)
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

		constexpr std::array vertexBindings = {
			vk::VertexInputBindingDescription(
				VERTEX_BUFFER_BINDING,
				sizeof(Vertex),
				vk::VertexInputRate::eVertex
			)
		};

		constexpr std::array vertexAttributes = {
			vk::VertexInputAttributeDescription(
				VERTEX_POSITION,
				VERTEX_BUFFER_BINDING,
				vk::Format::eR32G32Sfloat,
				offsetof(Vertex, position)
			),
			vk::VertexInputAttributeDescription(
				VERTEX_TEXCOORD,
				VERTEX_BUFFER_BINDING,
				vk::Format::eR32G32Sfloat,
				offsetof(Vertex, texCoord)
			)
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
	, VideoBase(instance.getApplicationInfo().defaultVideoMode)
	, m_impl(instance, findPad(std::get<0>(PADS)))
{
	setUpdateCallback(std::bind(&Impl::update, std::ref(*m_impl)));
	VideoBase::setVideoModeCompatibility(m_impl->getVideoModeCompatibility());
}

Window::Window(Instance& instance, std::string&& name)
	: ZuazoBase(instance, std::move(name), PADS)
	, VideoBase(instance.getApplicationInfo().defaultVideoMode)
	, m_impl(instance, findPad(std::get<0>(PADS)))
{
	setUpdateCallback(std::bind(&Impl::update, std::ref(*m_impl)));
	VideoBase::setVideoModeCompatibility(m_impl->getVideoModeCompatibility());
}

Window::Window(Window&& other) = default;

Window::~Window() {
	if(isOpen()) {
		close();
	}
}

Window& Window::operator=(Window&& other) = default;

void Window::open() {
	const auto& videoMode = getVideoMode();
	validate(videoMode);

	m_impl->open(videoMode, getName());
	ZuazoBase::enablePeriodicUpdate(Instance::OUTPUT_PRIORITY, 
									Timing::getPeriod(videoMode.get<VideoMode::Parameters::frameRate>()) );

	VideoBase::setVideoModeCompatibility(m_impl->getVideoModeCompatibility());
	ZuazoBase::open();
}

void Window::close() {
	ZuazoBase::disablePeriodicUpdate();
	m_impl->close();

	VideoBase::setVideoModeCompatibility(m_impl->getVideoModeCompatibility());
	ZuazoBase::close();
}

void Window::setVideoMode(const VideoMode& videoMode) {
	validate(videoMode);
	m_impl->setVideoMode(videoMode);

	if(	Timing::getPeriod(getVideoMode().get<VideoMode::Parameters::frameRate>()) !=
		Timing::getPeriod(videoMode.get<VideoMode::Parameters::frameRate>()) &&
		ZuazoBase::isOpen() )
	{
		//Update the update rate rate
		ZuazoBase::disablePeriodicUpdate();
		ZuazoBase::enablePeriodicUpdate(Instance::OUTPUT_PRIORITY, 
										Timing::getPeriod(videoMode.get<VideoMode::Parameters::frameRate>()) );
	}

	VideoBase::setVideoMode(videoMode);
}



void Window::setScalingMode(ScalingMode mode) {	
	m_impl->setScalingMode(mode);
}

ScalingMode Window::getScalingMode() const {
	return m_impl->getScalingMode();
}


void Window::setScalingFilter(ScalingFilter filter) {
	m_impl->setScalingFilter(filter);
}

ScalingFilter Window::getScalingFilter() const {
	return m_impl->getScalingFilter();
}

}