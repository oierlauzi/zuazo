#include <zuazo/Graphics/Drawtable.h>

#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/Buffer.h>
#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Utils/Pool.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/Exception.h>

#include <utility>
#include <memory>
#include <cassert>
#include <algorithm>

namespace Zuazo::Graphics {

/*
 * Drawtable::Impl
 */

struct Drawtable::Impl {
	class Allocator : public std::allocator<TargetFrame> {
	
	public:
		Allocator(const Impl& impl) 
			: m_drawtable(impl) 
		{
		}

		Allocator(const Allocator& other) = default;
		~Allocator() = default;

		Allocator& operator=(const Allocator& other) = default;

		void construct(TargetFrame* x) {
			const Impl& drw = m_drawtable;

			new (x) TargetFrame(
				drw.vulkan,
				drw.frameDescriptor,
				drw.colorTransferBuffer,
				drw.planeDescriptors,
				drw.depthStencil,
				drw.renderPass
			);
		}

	private:
		std::reference_wrapper<const Impl>				m_drawtable;

	};

	std::reference_wrapper<const Vulkan>			vulkan;
	std::shared_ptr<Frame::Descriptor>				frameDescriptor;
	InputColorTransfer								colorTransfer;
	std::shared_ptr<StagedBuffer>					colorTransferBuffer;
	std::vector<Frame::PlaneDescriptor>				planeDescriptors;

	std::shared_ptr<DepthStencil>					depthStencil;

	vk::RenderPass									renderPass;

	mutable Utils::Pool<TargetFrame, Allocator>		framePool;


	Impl(	const Vulkan& vulkan, 
			const Frame::Descriptor& frameDesc,
			DepthStencilFormat depthStencilFmt )
		: vulkan(vulkan)
		, frameDescriptor(Utils::makeShared<Frame::Descriptor>(frameDesc))
		, colorTransfer(*frameDescriptor)
		, colorTransferBuffer(Frame::createColorTransferBuffer(vulkan, colorTransfer))
		, planeDescriptors(createPlaneDescriptors(vulkan, frameDesc, colorTransfer))
		, depthStencil(createDepthStencil(vulkan, toVulkan(depthStencilFmt), planeDescriptors))
		, renderPass(getRenderPass(vulkan, frameDesc, depthStencilFmt))
		, framePool(1, Allocator(*this))
	{
	}

	~Impl() {
		colorTransferBuffer->waitCompletion(vulkan);
	}


	const Vulkan& getVulkan() const noexcept {
		return vulkan;
	}

	const Frame::Descriptor& getFrameDescriptor() const noexcept {
		assert(frameDescriptor);
		return *frameDescriptor;
	}



	void setMaxSpareCount(size_t spares) noexcept {
		framePool.setMaxSpareCount(spares);
	}

	size_t getMaxSpareCount() const noexcept {
		return framePool.getMaxSpareCount();
	}

	size_t getSpareCount() const noexcept {
		return framePool.getSpareCount();
	}


	std::shared_ptr<TargetFrame> acquireFrame() const {
		return framePool.acquire();
	}

	OutputColorTransfer getOutputColorTransfer() const {
		return OutputColorTransfer(colorTransfer);
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

			//Check if the conversion succeeded. For the moment only single-plane formats are supported
			//The code below would be multi-plane proof changing the if statement to endIte != conversion.cbegin()
			if(endIte == (conversion.cbegin() + 1)) {
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

			//Check if the conversion succeeded. For the moment only single-plane formats are supported
			//The code below would be multi-plane proof changing the if statement to endIte != conversion.cbegin()
			if(endIte == (conversion.cbegin() + 1)) {
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

	static vk::RenderPass getRenderPass(const Vulkan& vulkan, 
										const Frame::Descriptor& frameDesc,
										DepthStencilFormat depthStencilFmt )
	{
		//Get an id for this configuration
		static std::unordered_map<size_t, const Utils::StaticId> ids;
		const auto index =	static_cast<size_t>(frameDesc.getColorFormat()) +
							static_cast<size_t>(frameDesc.getColorTransferFunction())*static_cast<size_t>(ColorFormat::COUNT) +
							static_cast<size_t>(depthStencilFmt)*static_cast<size_t>(ColorFormat::COUNT)*static_cast<size_t>(ColorTransferFunction::COUNT);
		const size_t id = ids[index];

		//Check if a renderpass with this id exists
		vk::RenderPass result = vulkan.createRenderPass(id);
		if(!result) {
			//Get information about the image planes
			InputColorTransfer inputColorTransfer(frameDesc);
			const auto planeDescriptors = createPlaneDescriptors(vulkan, frameDesc, inputColorTransfer);
			const auto depthStencilFormat = toVulkan(depthStencilFmt);

			//Get the element count for the arrays
			const size_t colorAttachmentCount = planeDescriptors.size();
			const size_t depthStencilAttachmentCount = (depthStencilFormat == vk::Format::eUndefined) ? 0 : 1;
			const size_t attachmentCount = colorAttachmentCount + depthStencilAttachmentCount;

			//Create the attachments descriptors
			std::vector<vk::AttachmentDescription> attachments;
			attachments.reserve(attachmentCount);

			for(const auto& plane : planeDescriptors) {
				attachments.emplace_back(
					vk::AttachmentDescriptionFlags(),				//Flags
					plane.format,									//Attachemnt format
					vk::SampleCountFlagBits::e1,					//Sample count
					vk::AttachmentLoadOp::eClear,					//Color attachment load operation
					vk::AttachmentStoreOp::eStore,					//Color attachemnt store operation
					vk::AttachmentLoadOp::eDontCare,				//Stencil attachment load operation
					vk::AttachmentStoreOp::eDontCare,				//Stencil attachment store operation
					vk::ImageLayout::eUndefined,					//Initial layout
					vk::ImageLayout::eShaderReadOnlyOptimal			//Final layout
				);
			}

			if(depthStencilFormat != vk::Format::eUndefined) {
				attachments.emplace_back(
					vk::AttachmentDescriptionFlags(),				//Flags
					depthStencilFormat,								//Attachemnt format
					vk::SampleCountFlagBits::e1,					//Sample count
					vk::AttachmentLoadOp::eClear,					//Depth attachment load operation
					vk::AttachmentStoreOp::eDontCare,				//Depth attachemnt store operation
					vk::AttachmentLoadOp::eClear,					//Stencil attachment load operation
					vk::AttachmentStoreOp::eDontCare,				//Stencil attachment store operation
					vk::ImageLayout::eUndefined,					//Initial layout
					vk::ImageLayout::eDepthStencilAttachmentOptimal	//Final layout //TODO maybe undefined?
				);
			}
			assert(attachments.size() == attachmentCount);

			//Create the color attachment references for the subpass
			std::vector<vk::AttachmentReference> colorAttachmentReferences;
			colorAttachmentReferences.reserve(colorAttachmentCount);

			for(size_t i = 0; i < colorAttachmentCount; ++i) {
				colorAttachmentReferences.emplace_back(
					i, 												//Attachments index
					vk::ImageLayout::eColorAttachmentOptimal 		//Attachemnt layout
				);
			}
			assert(colorAttachmentReferences.size() == colorAttachmentCount);

			//Create the depth and stencil attachment reference
			const vk::AttachmentReference depthStencilAttachmentReference(
				depthStencilAttachmentCount ? colorAttachmentCount : VK_ATTACHMENT_UNUSED, 	//Attachments index
				vk::ImageLayout::eDepthStencilAttachmentOptimal 							//Attachemnt layout
			);

			//Create the subpass descriptor
			const std::array subpassDescriptors {
				vk::SubpassDescription(
					{},												//Flags
					vk::PipelineBindPoint::eGraphics,				//Pipeline bind point
					0, nullptr,										//Input attachments
					colorAttachmentReferences.size(), colorAttachmentReferences.data(), //Color attachments
					nullptr,										//Resolve attachment
					&depthStencilAttachmentReference, 				//Depth-stencil attachment
					0, nullptr										//Preserve attachments
				)
			};

			//Create subpass dependencies //TODO maybe remove
			constexpr vk::PipelineStageFlags subpassDependencyStages =
				vk::PipelineStageFlagBits::eColorAttachmentOutput 	|
				vk::PipelineStageFlagBits::eEarlyFragmentTests 		;
				
			constexpr vk::AccessFlags subpassDependencyDstAccess =
				vk::AccessFlagBits::eColorAttachmentWrite 			|
				vk::AccessFlagBits::eDepthStencilAttachmentWrite 	;

			constexpr std::array subpassDependencies {
				vk::SubpassDependency(
					VK_SUBPASS_EXTERNAL,							//Source subpass
					0,												//Destination subpass
					subpassDependencyStages,						//Source stage
					subpassDependencyStages,						//Destination stage
					{},												//Source access mask
					subpassDependencyDstAccess 						//Destintation access mask
				)
			};

			//Create the renderpass
			const vk::RenderPassCreateInfo createInfo(
				{},													//Flags
				attachments.size(), attachments.data(),				//Attachments
				subpassDescriptors.size(), subpassDescriptors.data(), //Subpasses
				subpassDependencies.size(), subpassDependencies.data() //Subpass dependencies
			);

			result = vulkan.createRenderPass(id, createInfo);
		}

		assert(result);
		return result;
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
	static const std::vector<vk::Format>& getVulkanFormatSupport(const Vulkan& vulkan) {
		constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
			vk::FormatFeatureFlagBits::eSampledImage |
			vk::FormatFeatureFlagBits::eColorAttachment |
			vk::FormatFeatureFlagBits::eColorAttachmentBlend ;

		return vulkan.listSupportedFormatsOptimal(DESIRED_FLAGS);
	}

	static const std::vector<vk::Format>& getVulkanFormatSupportDepthStencil(const Vulkan& vulkan) {
		constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
			vk::FormatFeatureFlagBits::eDepthStencilAttachment ;

		return vulkan.listSupportedFormatsOptimal(DESIRED_FLAGS);
	}

	static std::vector<Frame::PlaneDescriptor> createPlaneDescriptors(	const Vulkan& vulkan, 
																		const Frame::Descriptor& desc,
																		InputColorTransfer& colorTransfer )
	{
		std::vector<Frame::PlaneDescriptor> result = Frame::getPlaneDescriptors(desc);

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

	static std::shared_ptr<DepthStencil> createDepthStencil(const Vulkan& vulkan,
															vk::Format format,
															const std::vector<Frame::PlaneDescriptor>& desc)
	{
		std::shared_ptr<DepthStencil> result;

		//Check for support
		const auto& supportedFormats = getVulkanFormatSupportDepthStencil(vulkan);
		assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend())); //In order to use binary search
		const auto supported = std::binary_search(supportedFormats.cbegin(), supportedFormats.cend(), format);

		if(supported) {
			//We need to create a depth buffer
			assert(desc.size() > 0);

			result = Utils::makeShared<DepthStencil>(
				vulkan,
				desc.front().extent,
				format
			);
		}

		return result;
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
 * Drawtable
 */

Drawtable::Drawtable(	const Vulkan& vulkan, 
						const Frame::Descriptor& frameDesc,
						DepthStencilFormat depthStencilFmt )
	: m_impl({}, vulkan, frameDesc, depthStencilFmt)
{
}

Drawtable::Drawtable(Drawtable&& other) noexcept = default;

Drawtable::~Drawtable() = default;

Drawtable& Drawtable::operator=(Drawtable&& other) noexcept = default;



const Vulkan& Drawtable::getVulkan() const noexcept {
	return m_impl->getVulkan();
}

const Frame::Descriptor& Drawtable::getFrameDescriptor() const noexcept {
	return m_impl->getFrameDescriptor();
}


void Drawtable::setMaxSpareCount(size_t spares) noexcept {
	m_impl->setMaxSpareCount(spares);
}

size_t Drawtable::getMaxSpareCount() const noexcept {
	return m_impl->getMaxSpareCount();
}

size_t Drawtable::getSpareCount() const noexcept {
	return m_impl->getSpareCount();
}


std::shared_ptr<TargetFrame> Drawtable::acquireFrame() const {
	return m_impl->acquireFrame();
}

OutputColorTransfer Drawtable::getOutputColorTransfer() const {
	return m_impl->getOutputColorTransfer();
}


Utils::Discrete<ColorFormat> Drawtable::getSupportedFormats(const Vulkan& vulkan) {
	return Impl::getSupportedFormats(vulkan);
}

Utils::Discrete<ColorFormat> Drawtable::getSupportedSrgbFormats(const Vulkan& vulkan) {
	return Impl::getSupportedSrgbFormats(vulkan);
}

Utils::Discrete<DepthStencilFormat> Drawtable::getSupportedFormatsDepthStencil(const Vulkan& vulkan) {
	return Impl::getSupportedFormatsDepthStencil(vulkan);
}

vk::RenderPass Drawtable::getRenderPass(const Vulkan& vulkan, 
										const Frame::Descriptor& frameDesc,
										DepthStencilFormat depthStencilFmt )
{
	return Impl::getRenderPass(vulkan, frameDesc, depthStencilFmt);
}

std::vector<vk::ClearValue> Drawtable::getClearValues(	const Graphics::Frame::Descriptor& frameDesc,
														DepthStencilFormat depthStencilFmt )
{
	return Impl::getClearValues(frameDesc, depthStencilFmt);
}

}