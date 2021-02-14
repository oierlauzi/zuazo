#include <zuazo/Graphics/Drawtable.h>

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
				drw.samplers,
				drw.planeDescriptors,
				drw.depthStencil,
				drw.renderPass
			);
		}

	private:
		std::reference_wrapper<const Impl>				m_drawtable;

	};

	const Vulkan&									vulkan;
	InputColorTransfer								colorTransfer;
	std::shared_ptr<Frame::Descriptor>				frameDescriptor;
	std::vector<Image::Plane>						planeDescriptors;
	std::shared_ptr<Frame::Samplers>				samplers;
	std::shared_ptr<StagedBuffer>					colorTransferBuffer;

	std::shared_ptr<DepthStencil>					depthStencil;

	RenderPass										renderPass;

	mutable Utils::Pool<TargetFrame, Allocator>		framePool;


	Impl(	const Vulkan& vulkan, 
			const Frame::Descriptor& frameDesc,
			DepthStencilFormat depthStencilFmt )
		: vulkan(vulkan)
		, colorTransfer(frameDesc)
		, frameDescriptor(Utils::makeShared<Frame::Descriptor>(frameDesc))
		, planeDescriptors(createPlaneDescriptors(vulkan, frameDesc, colorTransfer))
		, samplers(Frame::createSamplers(vulkan, colorTransfer, planeDescriptors))
		, colorTransferBuffer(Frame::createColorTransferBuffer(vulkan, colorTransfer))
		, depthStencil(createDepthStencil(vulkan, toVulkan(depthStencilFmt), planeDescriptors))
		, renderPass(getRenderPass(vulkan, planeDescriptors, depthStencilFmt))
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

	OutputColorTransfer getOutputColorTransfer() const {
		return OutputColorTransfer(colorTransfer);
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

	static RenderPass getRenderPass(const Vulkan& vulkan, 
									const Frame::Descriptor& frameDesc,
									DepthStencilFormat depthStencilFmt )
	{
		InputColorTransfer inputColorTransfer(frameDesc);
		const auto planeDescriptors = createPlaneDescriptors(vulkan, frameDesc, inputColorTransfer);
		return RenderPass(vulkan, planeDescriptors, depthStencilFmt, vk::Format::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
	}	

private:
	static RenderPass getRenderPass(const Vulkan& vulkan, 
									Utils::BufferView<const Image::Plane> planeDescriptors,
									DepthStencilFormat depthStencilFmt )
	{
		return RenderPass(vulkan, planeDescriptors, depthStencilFmt, vk::Format::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

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

	static std::vector<Image::Plane> createPlaneDescriptors(const Vulkan& vulkan, 
															const Frame::Descriptor& desc,
															InputColorTransfer& colorTransfer )
	{
		auto result = Frame::getPlaneDescriptors(desc);

		//Try to optimize it
		const auto& supportedFormats = getVulkanFormatSupport(vulkan);
		assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend())); //In order to use binary search

		for(auto& plane : result) {
			const auto[format, swizzle] = optimizeFormat(std::make_tuple(plane.getFormat(), plane.getSwizzle()));
			plane.setFormat(format);
			plane.setSwizzle(swizzle);

			//Ensure that the format is supported
			assert(plane.getSwizzle() == vk::ComponentMapping());
			assert(std::binary_search(supportedFormats.cbegin(), supportedFormats.cend(), plane.getFormat()));
		}

		colorTransfer.optimize(result, supportedFormats);

		return result;
	}

	static std::shared_ptr<DepthStencil> createDepthStencil(const Vulkan& vulkan,
															vk::Format format,
															const std::vector<Image::Plane>& desc)
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
				to2D(desc.front().getExtent()),
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

const Frame::Descriptor& Drawtable::getFrameDescriptor() const noexcept {
	return m_impl->getFrameDescriptor();
}

OutputColorTransfer Drawtable::getOutputColorTransfer() const {
	return m_impl->getOutputColorTransfer();
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


Utils::Discrete<ColorFormat> Drawtable::getSupportedFormats(const Vulkan& vulkan) {
	return Impl::getSupportedFormats(vulkan);
}

Utils::Discrete<ColorFormat> Drawtable::getSupportedSrgbFormats(const Vulkan& vulkan) {
	return Impl::getSupportedSrgbFormats(vulkan);
}

Utils::Discrete<DepthStencilFormat> Drawtable::getSupportedFormatsDepthStencil(const Vulkan& vulkan) {
	return Impl::getSupportedFormatsDepthStencil(vulkan);
}

RenderPass Drawtable::getRenderPass(const Vulkan& vulkan, 
										const Frame::Descriptor& frameDesc,
										DepthStencilFormat depthStencilFmt )
{
	return Impl::getRenderPass(vulkan, frameDesc, depthStencilFmt);
}

}