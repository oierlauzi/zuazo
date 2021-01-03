#include <zuazo/Graphics/Downloader.h>

#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/Buffer.h>
#include <zuazo/Graphics/RenderPass.h>
#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Utils/Pool.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/shaders/color_transfer.h>

#include <utility>
#include <memory>
#include <cassert>
#include <algorithm>

namespace Zuazo::Graphics {

/*
 * Downloader::Impl
 */

struct Downloader::Impl {
	const Vulkan&									vulkan;
	std::shared_ptr<Frame::Descriptor>				frameDescriptor;
	InputColorTransfer								colorTransfer;
	std::vector<Image::PlaneDescriptor>				planeDescriptors;

	std::unique_ptr<DepthStencil>					depthStencil;
	RenderPass										renderPass;
	Image											targetImage;
	Framebuffer										framebuffer;
	Image											stagingImage;
	std::vector<Utils::BufferView<const std::byte>>	stagingImageData;

	vk::UniqueCommandPool							commandPool;
	vk::CommandBuffer								downloadCommand;
	vk::UniqueSemaphore								renderComplete;
	vk::UniqueFence									downloadComplete;

	std::shared_ptr<const CommandBuffer>			lastCommandBuffer;


	Impl(	const Vulkan& vulkan, 
			const Frame::Descriptor& frameDesc,
			DepthStencilFormat depthStencilFmt )
		: vulkan(vulkan)
		, frameDescriptor(Utils::makeShared<Frame::Descriptor>(frameDesc))
		, colorTransfer(*frameDescriptor)
		, planeDescriptors(createPlaneDescriptors(vulkan, frameDesc, colorTransfer))
		, depthStencil(createDepthStencil(vulkan, toVulkan(depthStencilFmt), planeDescriptors))
		, renderPass(getRenderPass(vulkan, planeDescriptors, depthStencilFmt))
		, targetImage(createTargetImage(vulkan, planeDescriptors))
		, framebuffer(createFramebuffer(vulkan, planeDescriptors, targetImage, depthStencil.get(), renderPass))
		, stagingImage(createStagingImage(vulkan, planeDescriptors))
		, stagingImageData(getPixelData(vulkan, stagingImage))
		, commandPool(createCommandPool(vulkan))
		, downloadCommand(createCommandBuffer(vulkan, *commandPool))
		, renderComplete(vulkan.createSemaphore())
		, downloadComplete(vulkan.createFence(true))
		, lastCommandBuffer()
	{
		recordCommandBuffer();
	}

	~Impl() {
		waitCompletion(Vulkan::NO_TIMEOUT);
	}


	const Vulkan& getVulkan() const noexcept {
		return vulkan;
	}

	const Frame::Descriptor& getFrameDescriptor() const noexcept {
		assert(frameDescriptor);
		return *frameDescriptor;
	}

	OutputColorTransfer getOutputColorTransfer() const {
		return OutputColorTransfer(colorTransfer);
	}


	const Framebuffer& getFramebuffer() const noexcept {
		return framebuffer;
	}

	void beginRenderPass(	vk::CommandBuffer cmd, 
							vk::Rect2D renderArea,
							Utils::BufferView<const vk::ClearValue> clearValues,
							vk::SubpassContents contents ) const noexcept
	{
		const vk::RenderPassBeginInfo beginInfo(
			renderPass.get(),
			framebuffer.get(),
			renderArea,
			clearValues.size(), clearValues.data()
		);

		getVulkan().beginRenderPass(cmd, beginInfo, contents);
	}

	void endRenderPass(vk::CommandBuffer cmd) const noexcept {
		vulkan.endRenderPass(cmd);
	}

	void draw(std::shared_ptr<const CommandBuffer> cmd) {
		//No previous download should be in progress
		waitCompletion(Vulkan::NO_TIMEOUT);

		lastCommandBuffer = std::move(cmd);
		assert(lastCommandBuffer);

		//Submit drawing commands to the graphics queue
		const std::array renderCommandbuffers = {
			lastCommandBuffer->get()
		};

		const std::array intermediateSemaphores = {
			*renderComplete
		};

		const vk::SubmitInfo renderSubmitInfo(
			0,								nullptr,	nullptr,			//Wait semaphores
			renderCommandbuffers.size(), 	renderCommandbuffers.data(),	//Command buffers
			intermediateSemaphores.size(), 	intermediateSemaphores.data()	//Signal semaphores
		);

		vulkan.submit(vulkan.getGraphicsQueue(), renderSubmitInfo, vk::Fence());

		//Submit download commands to the transfer queue
		constexpr std::array downloadWaitStages = {
			vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		};    
		static_assert(intermediateSemaphores.size() == downloadWaitStages.size(), "Wait semaphore sizes do not match");

		const std::array downloadCommandbuffers = {
			downloadCommand
		};

		const vk::SubmitInfo dowloadSubmitInfo(
			intermediateSemaphores.size(),	intermediateSemaphores.data(),	downloadWaitStages.data(),	//Wait semaphores
			downloadCommandbuffers.size(), 	downloadCommandbuffers.data(),								//Command buffers
			0, 								nullptr														//Signal semaphores
		);

		vulkan.resetFences(*downloadComplete);
		vulkan.submit(vulkan.getTransferQueue(), renderSubmitInfo, *downloadComplete);
	}

	bool waitCompletion(uint64_t timeo) const {
		return vulkan.waitForFences(*downloadComplete, true, timeo);
	}

	Utils::BufferView<const Utils::BufferView<const std::byte>>	getPixelData() const noexcept {
		return stagingImageData;
	}


	static Utils::Discrete<ColorFormat> getSupportedFormats(const Vulkan& vulkan) {
		Utils::Discrete<ColorFormat> result;

		//Query support for Vulkan formats
		const auto& vulkanFormatSupport = getVulkanFormatSupport(vulkan);
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

			//Check if the conversion succeeded
			if(endIte != conversion.cbegin()) {
				//Check if it is supported
				const auto supported = std::all_of(
					conversion.cbegin(), endIte,
					[&vulkanFormatSupport] (const std::tuple<vk::Format, vk::ComponentMapping>& conv) -> bool {
						//Try to optimize the format (remove any swizzle if possible)
						const auto optimized = optimizeFormat(conv);

						//Check if it has any swizzle
						if(std::get<1>(optimized) != vk::ComponentMapping()) return false;

						//Check if the format is supported indeed
						return std::binary_search(vulkanFormatSupport.cbegin(), vulkanFormatSupport.cend(), std::get<0>(optimized));
					}
				);

				if(supported) {
					result.push_back(i);
				}
			}
		}

		return result;
	}

	static Utils::Discrete<ColorFormat> getSupportedSrgbFormats(const Vulkan& vulkan) {
		Utils::Discrete<ColorFormat> result;

		//Query support for Vulkan formats
		const auto& vulkanFormatSupport = getVulkanFormatSupport(vulkan);
		assert(std::is_sorted(vulkanFormatSupport.cbegin(), vulkanFormatSupport.cend())); //For binary search

		//Test for each format
		for(auto i = Utils::EnumTraits<ColorFormat>::first(); i <= Utils::EnumTraits<ColorFormat>::last(); ++i) {
			//Convert it into a Vulkan format
			auto conversion = toVulkan(i);

			//Find the end of the range
			const auto endIte = std::find_if(
				conversion.cbegin(), conversion.cend(),
				[] (const std::tuple<vk::Format, vk::ComponentMapping>& conv) -> bool {
					return std::get<0>(conv) == vk::Format::eUndefined;
				}
			);

			//Check if the conversion succeeded
			if(endIte != conversion.cbegin()) {
				//Check if it is supported
				const auto supported = std::all_of(
					conversion.cbegin(), endIte,
					[&vulkanFormatSupport] (const std::tuple<vk::Format, vk::ComponentMapping>& conv) -> bool {
						//Try to convert it to sRGB
						const auto sRGBfmt = toSrgb(std::get<0>(conv));
						if(sRGBfmt == std::get<0>(conv)) return false; //No sRGB equivalent

						//Try to optimize the format (remove any swizzle if possible)
						const auto optimized = optimizeFormat(std::make_tuple(sRGBfmt, std::get<1>(conv)));

						//Check if it has any swizzle
						if(std::get<1>(optimized) != vk::ComponentMapping()) return false;

						//Check if the format is supported indeed
						return std::binary_search(vulkanFormatSupport.cbegin(), vulkanFormatSupport.cend(), std::get<0>(optimized));
					}
				);

				if(supported) {
					result.push_back(i);
				}
			}
		}

		return result;
	}

	static Utils::Discrete<DepthStencilFormat> getSupportedFormatsDepthStencil(const Vulkan& vulkan) {
		Utils::Discrete<DepthStencilFormat> result;

		//Query support for Vulkan formats
		const auto& vulkanFormatSupport = getVulkanFormatSupportDepthStencil(vulkan);
		assert(std::is_sorted(vulkanFormatSupport.cbegin(), vulkanFormatSupport.cend())); //For binary search

		//Having no depth/stencil is supported:
		result.emplace_back(DepthStencilFormat::NONE);

		//Test for each format
		for(auto i = Utils::EnumTraits<DepthStencilFormat>::first(); i <= Utils::EnumTraits<DepthStencilFormat>::last(); ++i) {
			//Convert it into a Vulkan format
			const auto conversion = toVulkan(i);

			//Check if it is supported
			const auto supported = std::binary_search(
				vulkanFormatSupport.cbegin(), vulkanFormatSupport.cend(),
				conversion
			);

			if(supported) {
				result.push_back(i);
			}
		}

		return result;
	}	

	static RenderPass getRenderPass(const Vulkan& vulkan, 
									const Frame::Descriptor& frameDesc,
									DepthStencilFormat depthStencilFmt )
	{
		InputColorTransfer inputColorTransfer(frameDesc);
		const auto planeDescriptors = createPlaneDescriptors(vulkan, frameDesc, inputColorTransfer);
		return RenderPass(vulkan, planeDescriptors, depthStencilFmt, vk::ImageLayout::eTransferSrcOptimal);
	}
	
	static std::vector<vk::ClearValue> getClearValues(	const Graphics::Frame::Descriptor& frameDesc,
														DepthStencilFormat depthStencilFmt )
	{
		std::vector<vk::ClearValue> result;

		//Obtain information about the attachments
		const auto coloAttachmentCount = getPlaneCount(frameDesc.getColorFormat());
		const auto hasDepthStencil = DepthStencilFormat::NONE < depthStencilFmt && depthStencilFmt < DepthStencilFormat::COUNT;
		assert(coloAttachmentCount > 0 && coloAttachmentCount <= 4);
		result.reserve(coloAttachmentCount + hasDepthStencil);

		//Add the color attachment clear values
		if(isYCbCr(frameDesc.getColorModel())) {
			for(size_t i = 0; i < coloAttachmentCount; ++i) {
				result.emplace_back(getYCbCrClearValues(frameDesc.getColorFormat(), i));
			}
		} else {
			for(size_t i = 0; i < coloAttachmentCount; ++i) {
				result.emplace_back(vk::ClearColorValue()); //Default initializer to 0 of floats (Unorm)
			}
		}

		//Add the depth/stencil attachemnt clear values
		if(hasDepthStencil) {
			result.emplace_back(vk::ClearDepthStencilValue(1.0f, 0x00)); //Clear to the far plane
		}

		return result;
	}


private:
	void recordCommandBuffer() {
		const auto& srcImage = targetImage;
		const auto& dstImage = stagingImage;
		const auto cmd = downloadCommand;

		const bool queueOwnershipTransfer = vulkan.getGraphicsQueueIndex() != vulkan.getTransferQueueIndex();
		const size_t planeCount = planeDescriptors.size();
		const size_t barrierCount = 2*planeCount;

		assert(planeCount == srcImage.getPlanes().size());
		assert(planeCount == dstImage.getPlanes().size());
		
		//Record the command buffer
		const vk::CommandBufferBeginInfo beginInfo(
			{},
			nullptr
		);

		vulkan.begin(cmd, beginInfo);
		{
			std::vector<vk::ImageMemoryBarrier> memoryBarriers;
			memoryBarriers.reserve(barrierCount);

			constexpr vk::ImageSubresourceRange imageSubresourceRange(
				vk::ImageAspectFlagBits::eColor,				//Aspect mask
				0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
			);

			//Transition the layout of the images
			{
				memoryBarriers.clear();
				for(const auto& plane : srcImage.getPlanes()){
					constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eColorAttachmentWrite;
					constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferRead;

					//Do not change the layout
					constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eTransferSrcOptimal;
					constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eTransferSrcOptimal;

					//Handle the image to the transfer queue
					const auto srcFamily = queueOwnershipTransfer ? vulkan.getGraphicsQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
					const auto dstFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;

					memoryBarriers.emplace_back(
						srcAccess,								//Old access mask
						dstAccess,								//New access mask
						srcLayout,								//Old layout
						dstLayout,								//New layout
						srcFamily,								//Old queue family
						dstFamily,								//New queue family
						*plane.image,							//Image
						imageSubresourceRange					//Image subresource
					);
				}
				for(const auto& plane : dstImage.getPlanes()){
					constexpr vk::AccessFlags srcAccess = {};
					constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferWrite;

					constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eUndefined;
					constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eTransferDstOptimal;

					//Staging frame always owned by the transfer queue
					constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
					constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

					memoryBarriers.emplace_back(
						srcAccess,								//Old access mask
						dstAccess,								//New access mask
						srcLayout,								//Old layout
						dstLayout,								//New layout
						srcFamily,								//Old queue family
						dstFamily,								//New queue family
						*plane.image,							//Image
						imageSubresourceRange					//Image subresource
					);
				}
				assert(memoryBarriers.size() == barrierCount);

				constexpr vk::PipelineStageFlags srcStages = 
					vk::PipelineStageFlagBits::eAllGraphics;

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
			for(size_t i = 0; i < planeCount; i++){
				constexpr vk::ImageSubresourceLayers imageSubresourceLayers(
					imageSubresourceRange.aspectMask,					//Aspect mask
					imageSubresourceRange.baseMipLevel,					//Mip level
					imageSubresourceRange.baseArrayLayer,				//Array layer offset
					imageSubresourceRange.layerCount 					//Array layers
				);

				const vk::ImageCopy region(
					imageSubresourceLayers,								//Src subresource
					vk::Offset3D(),										//Src offset
					imageSubresourceLayers,								//Dst subresource
					vk::Offset3D(),										//Dst offset
					vk::Extent3D(planeDescriptors[i].extent, 1)			//Image extent
				);

				vulkan.copy(
					cmd,
					*(srcImage.getPlanes()[i].image),
					vk::ImageLayout::eTransferSrcOptimal,
					*(dstImage.getPlanes()[i].image),
					vk::ImageLayout::eTransferDstOptimal,
					region
				);
			}

			//Transition the layout of the images (again)
			{
				memoryBarriers.clear();
				for(const auto& plane : srcImage.getPlanes()){
					constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferRead;
					constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eColorAttachmentWrite;

					//Do not change the layout
					constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eTransferSrcOptimal;
					constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eTransferSrcOptimal;
					
					//Handle the image back to the graphics queue
					const auto srcFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
					const auto dstFamily = queueOwnershipTransfer ? vulkan.getGraphicsQueueIndex() : VK_QUEUE_FAMILY_IGNORED;

					memoryBarriers.emplace_back(
						srcAccess,								//Old access mask
						dstAccess,								//New access mask
						srcLayout,								//Old layout
						dstLayout,								//New layout
						srcFamily,								//Old queue family
						dstFamily,								//New queue family
						*plane.image,							//Image
						imageSubresourceRange					//Image subresource
					);
				}
				for(const auto& plane : dstImage.getPlanes()){
					constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferWrite;
					constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eHostRead;

					constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eTransferDstOptimal;
					constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eGeneral;

					//Staging frame always owned by the transfer queue
					constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
					constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

					memoryBarriers.emplace_back(
						srcAccess,								//Old access mask
						dstAccess,								//New access mask
						srcLayout,								//Old layout
						dstLayout,								//New layout
						srcFamily,								//Old queue family
						dstFamily,								//New queue family
						*plane.image,							//Image
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
		vulkan.end(cmd);
	}

	static std::vector<Image::PlaneDescriptor> createPlaneDescriptors(	const Vulkan& vulkan, 
																		const Frame::Descriptor& desc,
																		InputColorTransfer& colorTransfer )
	{
		std::vector<Image::PlaneDescriptor> result = Frame::getPlaneDescriptors(desc);

		//Try to optimize it
		const auto& supportedFormats = getVulkanFormatSupport(vulkan);
		assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend())); //In order to use binary search

		for(auto& plane : result) {
			std::tie(plane.format, plane.swizzle) = optimizeFormat(std::make_tuple(plane.format, plane.swizzle));

			//Ensure that the format is supported
			assert(plane.swizzle == vk::ComponentMapping());
			assert(std::binary_search(supportedFormats.cbegin(), supportedFormats.cend(), plane.format));
		}

		colorTransfer.optimize(result, supportedFormats);

		return result;
	}

	static std::unique_ptr<DepthStencil> createDepthStencil(const Vulkan& vulkan,
															vk::Format format,
															const std::vector<Image::PlaneDescriptor>& desc)
	{
		std::unique_ptr<DepthStencil> result;

		//Check for support
		const auto& supportedFormats = getVulkanFormatSupportDepthStencil(vulkan);
		assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend())); //In order to use binary search
		const auto supported = std::binary_search(supportedFormats.cbegin(), supportedFormats.cend(), format);

		if(supported) {
			//We need to create a depth buffer
			assert(desc.size() > 0);

			result = Utils::makeUnique<DepthStencil>(
				vulkan,
				desc.front().extent,
				format
			);
		}

		return result;
	}

	Image createTargetImage(const Vulkan& vulkan,
							Utils::BufferView<const Image::PlaneDescriptor> planes )
	{
		constexpr vk::ImageUsageFlags usage =
			vk::ImageUsageFlagBits::eColorAttachment |
			vk::ImageUsageFlagBits::eTransferSrc;

		constexpr vk::ImageTiling tiling =
			vk::ImageTiling::eOptimal;

		constexpr vk::MemoryPropertyFlags memory =
			vk::MemoryPropertyFlagBits::eDeviceLocal;

		return Image(
			vulkan,
			planes,
			usage,
			tiling,
			memory
		);
	}

	static Framebuffer createFramebuffer(	const Vulkan& vulkan,
											Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
											const Image& image,
											const DepthStencil* depthStencil,
											RenderPass renderPass )
	{
		return Framebuffer(
			vulkan,
			planeDescriptors,
			image,
			depthStencil,
			renderPass
		);
	}

	Image createStagingImage(	const Vulkan& vulkan,
								Utils::BufferView<const Image::PlaneDescriptor> planes )
	{
		constexpr vk::ImageUsageFlags usage =
			vk::ImageUsageFlagBits::eTransferDst;

		constexpr vk::ImageTiling tiling =
			vk::ImageTiling::eLinear;

		constexpr vk::MemoryPropertyFlags memory =
			vk::MemoryPropertyFlagBits::eHostVisible;

		return Image(
			vulkan,
			planes,
			usage,
			tiling,
			memory
		);
	}

	static std::vector<Utils::BufferView<const std::byte>> getPixelData(const Vulkan& vulkan,
																		const Image& stagingImage )
	{
		const vk::MappedMemoryRange range(
			*(stagingImage.getMemory().memory),
			0, VK_WHOLE_SIZE
		);

		auto* data = reinterpret_cast<const std::byte*>(vulkan.mapMemory(range));
		return Utils::slice(data, stagingImage.getMemory().areas);
	}

	static vk::UniqueCommandPool createCommandPool(const Vulkan& vulkan) {
		const vk::CommandPoolCreateInfo createInfo(
			{},
			vulkan.getGraphicsQueueIndex()
		);

		return vulkan.createCommandPool(createInfo);
	}

	static vk::CommandBuffer createCommandBuffer(const Vulkan& vulkan, vk::CommandPool pool) {
		return vulkan.allocateCommnadBuffer(pool, vk::CommandBufferLevel::ePrimary).release();
	}

	static RenderPass getRenderPass(const Vulkan& vulkan, 
									Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
									DepthStencilFormat depthStencilFmt )
	{
		return RenderPass(vulkan, planeDescriptors, depthStencilFmt, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	static const std::vector<vk::Format>& getVulkanFormatSupport(const Vulkan& vulkan) {
		constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
			vk::FormatFeatureFlagBits::eSampledImage |
			vk::FormatFeatureFlagBits::eColorAttachment |
			vk::FormatFeatureFlagBits::eColorAttachmentBlend |
			vk::FormatFeatureFlagBits::eTransferSrc ;

		return vulkan.listSupportedFormatsOptimal(DESIRED_FLAGS);
	}

	static const std::vector<vk::Format>& getVulkanFormatSupportDepthStencil(const Vulkan& vulkan) {
		constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
			vk::FormatFeatureFlagBits::eDepthStencilAttachment ;

		return vulkan.listSupportedFormatsOptimal(DESIRED_FLAGS);
	}

	static std::array<float, 4> getYCbCrClearValues(ColorFormat format, uint32_t plane) {
		const auto planeCount = getPlaneCount(format);
		assert(plane < planeCount);

		if(planeCount > 1) {
			//Multiplanar format. Decide what are the contents of each plane
			//Note, the following table may have some misleading values. They are there 
			//so that the compiler may optimise it to a smoother LUT. As practically speaking
			//they wont be read. They have been commented with the component name followed 
			//by an asterisk (*)
			switch(plane) {
			case 1:
				switch(format) {
				case ColorFormat::G8_B8_R8_A8:
				case ColorFormat::G10X6_B10X6_R10X6_A10X6_16:
				case ColorFormat::G12X4_B12X4_R12X4_A12X4_16:
				case ColorFormat::G16_B16_R16_A16:
				case ColorFormat::G16f_B16f_R16f_A16f:
				case ColorFormat::G32f_B32f_R32f_A32f:
				case ColorFormat::G64f_B64f_R64f_A64f:
				case ColorFormat::G8_B8_R8:
				case ColorFormat::G10X6_B10X6_R10X6_16:
				case ColorFormat::G12X4_B12X4_R12X4_16:
				case ColorFormat::G16_B16_R16:
				case ColorFormat::G16f_B16f_R16f:
				case ColorFormat::G32f_B32f_R32f:
				case ColorFormat::G64f_B64f_R64f:
					return {0.5f, 0.5f, 0.0f, 0.0f}; //Cb on R, put it to .5. The rest won't be read. G*
				case ColorFormat::G8_R8B8:
				case ColorFormat::G8_B8R8:
				case ColorFormat::G10X6_R10X6B10X6_16:
				case ColorFormat::G10X6_B10X6R10X6_16:
				case ColorFormat::G12X4_R12X4B12X4_16:
				case ColorFormat::G12X4_B12X4R12X4_16:
				case ColorFormat::G16_R16B16:
				case ColorFormat::G16_B16R16:
					return {0.5f, 0.5f, 0.0f, 0.0f}; //Cb/Cr on R, Cb/Cr on G put them to .5. The rest won't be read
				default:
					assert(false);
					return {0.5f, 0.5f, 0.0f, 0.0f}; //Unknown format. R*G*
				}
			case 2:
				switch(format) {
				case ColorFormat::G8_B8_R8_A8:
				case ColorFormat::G10X6_B10X6_R10X6_A10X6_16:
				case ColorFormat::G12X4_B12X4_R12X4_A12X4_16:
				case ColorFormat::G16_B16_R16_A16:
				case ColorFormat::G16f_B16f_R16f_A16f:
				case ColorFormat::G32f_B32f_R32f_A32f:
				case ColorFormat::G64f_B64f_R64f_A64f:
				case ColorFormat::G8_B8_R8:
				case ColorFormat::G10X6_B10X6_R10X6_16:
				case ColorFormat::G12X4_B12X4_R12X4_16:
				case ColorFormat::G16_B16_R16:
				case ColorFormat::G16f_B16f_R16f:
				case ColorFormat::G32f_B32f_R32f:
				case ColorFormat::G64f_B64f_R64f:
					return {0.5f, 0.5f, 0.0f, 0.0f}; //Cr on R, put it to .5. The rest won't be read. G*
				default:
					assert(false);
					return {0.5f, 0.5f, 0.0f, 0.0f}; //Unknown format R*G*
				}
			case 3:
				switch(format) {
				case ColorFormat::G8_B8_R8_A8:
				case ColorFormat::G10X6_B10X6_R10X6_A10X6_16:
				case ColorFormat::G12X4_B12X4_R12X4_A12X4_16:
				case ColorFormat::G16_B16_R16_A16:
				case ColorFormat::G16f_B16f_R16f_A16f:
				case ColorFormat::G32f_B32f_R32f_A32f:
				case ColorFormat::G64f_B64f_R64f_A64f:
					return {0.0f, 0.0f, 0.0f, 0.0f}; //Alpha plane
				default:
					assert(false);
					return {0.0f, 0.0f, 0.0f, 0.0f}; //Unknown format
				}
			default: //0
				switch(format) {
				case ColorFormat::G8_B8_R8_A8:
				case ColorFormat::G10X6_B10X6_R10X6_A10X6_16:
				case ColorFormat::G12X4_B12X4_R12X4_A12X4_16:
				case ColorFormat::G16_B16_R16_A16:
				case ColorFormat::G16f_B16f_R16f_A16f:
				case ColorFormat::G32f_B32f_R32f_A32f:
				case ColorFormat::G64f_B64f_R64f_A64f:
				case ColorFormat::G8_B8_R8:
				case ColorFormat::G10X6_B10X6_R10X6_16:
				case ColorFormat::G12X4_B12X4_R12X4_16:
				case ColorFormat::G16_B16_R16:
				case ColorFormat::G16f_B16f_R16f:
				case ColorFormat::G32f_B32f_R32f:
				case ColorFormat::G64f_B64f_R64f:
				case ColorFormat::G8_R8B8:
				case ColorFormat::G8_B8R8:
				case ColorFormat::G10X6_R10X6B10X6_16:
				case ColorFormat::G10X6_B10X6R10X6_16:
				case ColorFormat::G12X4_R12X4B12X4_16:
				case ColorFormat::G12X4_B12X4R12X4_16:
				case ColorFormat::G16_R16B16:
				case ColorFormat::G16_B16R16:
					return {0.0f, 0.0f, 0.0f, 0.0f}; //Allways the luminance plane
				default:
					assert(false);
					return {0.0f, 0.0f, 0.0f, 0.0f}; //Unknown format
				}
			}
		} else {
			//Packed format
			return {0.5f, 0.0f, 0.5f, 0.0f}; //Cr Y Cb A.
		}
	}

};



/*
 * Downloader
 */

Downloader::Downloader(	const Vulkan& vulkan, 
						const Frame::Descriptor& frameDesc,
						DepthStencilFormat depthStencilFmt )
	: m_impl({}, vulkan, frameDesc, depthStencilFmt)
{
}

Downloader::Downloader(Downloader&& other) noexcept = default;

Downloader::~Downloader() = default;

Downloader& Downloader::operator=(Downloader&& other) noexcept = default;



const Vulkan& Downloader::getVulkan() const noexcept {
	return m_impl->getVulkan();
}

const Frame::Descriptor& Downloader::getFrameDescriptor() const noexcept {
	return m_impl->getFrameDescriptor();
}

OutputColorTransfer Downloader::getOutputColorTransfer() const {
	return m_impl->getOutputColorTransfer();
}


const Framebuffer& Downloader::getFramebuffer() const noexcept {
	return m_impl->getFramebuffer();
}

void Downloader::beginRenderPass(	vk::CommandBuffer cmd, 
									vk::Rect2D renderArea,
									Utils::BufferView<const vk::ClearValue> clearValues,
									vk::SubpassContents contents ) const noexcept
{
	m_impl->beginRenderPass(cmd, renderArea, clearValues, contents);
}

void Downloader::endRenderPass(vk::CommandBuffer cmd) const noexcept {
	m_impl->endRenderPass(cmd);
}

void Downloader::draw(std::shared_ptr<const CommandBuffer> cmd) {
	m_impl->draw(std::move(cmd));
}

bool Downloader::waitCompletion(uint64_t timeo) const {
	return m_impl->waitCompletion(timeo);
}

Downloader::PixelData Downloader::getPixelData() const noexcept {
	return m_impl->getPixelData();
}


Utils::Discrete<ColorFormat> Downloader::getSupportedFormats(const Vulkan& vulkan) {
	return Impl::getSupportedFormats(vulkan);
}

Utils::Discrete<ColorFormat> Downloader::getSupportedSrgbFormats(const Vulkan& vulkan) {
	return Impl::getSupportedSrgbFormats(vulkan);
}

Utils::Discrete<DepthStencilFormat> Downloader::getSupportedFormatsDepthStencil(const Vulkan& vulkan) {
	return Impl::getSupportedFormatsDepthStencil(vulkan);
}

RenderPass Downloader::getRenderPass(const Vulkan& vulkan, 
										const Frame::Descriptor& frameDesc,
										DepthStencilFormat depthStencilFmt )
{
	return Impl::getRenderPass(vulkan, frameDesc, depthStencilFmt);
}

std::vector<vk::ClearValue> Downloader::getClearValues(	const Graphics::Frame::Descriptor& frameDesc,
														DepthStencilFormat depthStencilFmt )
{
	return Impl::getClearValues(frameDesc, depthStencilFmt);
}

}