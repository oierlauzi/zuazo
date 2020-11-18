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

			//Check if it is supported
			const auto supported = std::all_of(
				conversion.cbegin(), endIte,
				[&vulkanFormatSupport] (const std::tuple<vk::Format, vk::ComponentMapping>& conv) -> bool {
					//Try to optimize the format (remove any swizzle if possible)
					const auto optimized = optimizeFormat(conv);

					//Check if it has any swizzle
					if(std::get<1>(optimized) != vk::ComponentMapping()) return false;

					//Check if the format is supported indeed
					return std::binary_search(vulkanFormatSupport.cbegin(), vulkanFormatSupport.cend(), std::get<0>(conv));
				}
			);

			if(supported) {
				result.push_back(i);
			}
		}

		return result;
	}

	static Utils::Discrete<DepthStencilFormat> getSupportedFormatsDepthStencil(const Vulkan& vulkan) {
		Utils::Discrete<DepthStencilFormat> result;

		//Query support for Vulkan formats
		const auto& vulkanFormatSupport = getVulkanFormatSupportDepthStencil(vulkan);
		assert(std::is_sorted(vulkanFormatSupport.cbegin(), vulkanFormatSupport.cend())); //For binary search

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
		const auto index =	static_cast<size_t>(frameDesc.colorFormat) +
							static_cast<size_t>(frameDesc.colorTransferFunction)*static_cast<size_t>(ColorFormat::COUNT) +
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
				colorAttachmentCount, 							//Attachments index
				vk::ImageLayout::eDepthStencilAttachmentOptimal //Attachemnt layout
			);

			//Create the subpass descriptor
			const std::array subpassDescriptors {
				vk::SubpassDescription(
					{},												//Flags
					vk::PipelineBindPoint::eGraphics,				//Pipeline bind point
					0, nullptr,										//Input attachments
					colorAttachmentReferences.size(), colorAttachmentReferences.data(), //Color attachments
					nullptr,										//Resolve attachment
					colorAttachmentCount ? &depthStencilAttachmentReference : nullptr, //Depth-stencil attachment
					0, nullptr										//Preserve attachments
				)
			};

			//Create subpass dependencies
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

Utils::Discrete<DepthStencilFormat> Drawtable::getSupportedFormatsDepthStencil(const Vulkan& vulkan) {
	return Impl::getSupportedFormatsDepthStencil(vulkan);
}

vk::RenderPass Drawtable::getRenderPass(const Vulkan& vulkan, 
										const Frame::Descriptor& frameDesc,
										DepthStencilFormat depthStencilFmt )
{
	return Impl::getRenderPass(vulkan, frameDesc, depthStencilFmt);
}

}