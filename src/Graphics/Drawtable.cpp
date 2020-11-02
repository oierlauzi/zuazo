#include <zuazo/Graphics/Drawtable.h>

#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/Buffer.h>
#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Utils/Pool.h>
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
				drw.renderPass,
				drw.depthStencil
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

	std::shared_ptr<const vk::UniqueRenderPass> 	renderPass;
	std::shared_ptr<DepthStencil>					depthStencil;

	mutable Utils::Pool<TargetFrame, Allocator>		framePool;


	Impl(	const Vulkan& vulkan, 
			const Frame::Descriptor& conf,
			std::shared_ptr<const vk::UniqueRenderPass> renderPass,
			vk::Format depthStencilFmt )
		: vulkan(vulkan)
		, frameDescriptor(Utils::makeShared<Frame::Descriptor>(conf))
		, colorTransfer(*frameDescriptor)
		, colorTransferBuffer(Frame::createColorTransferBuffer(vulkan, colorTransfer))
		, planeDescriptors(createPlaneDescriptors(vulkan, conf, colorTransfer))
		, renderPass(std::move(renderPass))
		, depthStencil(createDepthStencil(vulkan, depthStencilFmt, planeDescriptors))
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


	static std::vector<ColorFormat> getSupportedFormats(const Vulkan& vulkan) {
		std::vector<ColorFormat> result;

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

		static std::vector<DepthStencilFormat> getSupportedFormatsDepthStencil(const Vulkan& vulkan) {
		std::vector<DepthStencilFormat> result;

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
						const Frame::Descriptor& conf,
						std::shared_ptr<const vk::UniqueRenderPass> renderPass,
						vk::Format depthStencilFmt )
	: m_impl({}, vulkan, conf, std::move(renderPass), depthStencilFmt)
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


std::vector<ColorFormat> Drawtable::getSupportedFormats(const Vulkan& vulkan) {
	return Impl::getSupportedFormats(vulkan);
}

std::vector<DepthStencilFormat> Drawtable::getSupportedFormatsDepthStencil(const Vulkan& vulkan) {
	return Impl::getSupportedFormatsDepthStencil(vulkan);
}


}