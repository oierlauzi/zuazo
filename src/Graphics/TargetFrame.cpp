#include <zuazo/Graphics/TargetFrame.h>

#include <zuazo/Graphics/RenderPass.h>
#include <zuazo/Graphics/ColorTransfer.h>

#include <algorithm>

namespace Zuazo::Graphics {

/*
 * Format support
 */

static const std::vector<vk::Format>& getVulkanFormatSupport(const Vulkan& vulkan) {
	constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
		vk::FormatFeatureFlagBits::eSampledImage |
		vk::FormatFeatureFlagBits::eColorAttachment |
		vk::FormatFeatureFlagBits::eColorAttachmentBlend ;

	return vulkan.listSupportedFormatsOptimal(DESIRED_FLAGS);
}





/*
 * TargetFrame::Cache
 */

class TargetFrame::Cache {
public:
	Cache(	const Vulkan& vulkan,
			const Frame::Descriptor& frameDesc,
			DepthStencilFormat depthStencilFmt )
		: m_vulkan(vulkan)
		, m_colorTransfer(frameDesc)
		, m_plane(getPlane(vulkan, frameDesc, m_colorTransfer))
		, m_renderPass(createRenderPass(vulkan, depthStencilFmt, m_colorTransfer, m_plane))
		, m_frameCache(Frame::createCache(vulkan, m_plane))
	{
	}

	~Cache() = default;

	const Vulkan& getVulkan() const noexcept{
		return m_vulkan;
	}

	const Image::Plane& getPlane() const noexcept {
		return m_plane;
	}

	const RenderPass& getRenderPass() const noexcept {
		return m_renderPass;
	}

	const std::shared_ptr<const Frame::Cache>& getFrameCache() const noexcept {
		return m_frameCache;
	}


private:
	std::reference_wrapper<const Vulkan> 	m_vulkan;

	ColorTransferWrite						m_colorTransfer;
	Image::Plane							m_plane;
	RenderPass								m_renderPass;

	std::shared_ptr<const Frame::Cache>		m_frameCache;



	static Image::Plane	getPlane(	const Vulkan& vulkan,
									const Frame::Descriptor& frameDesc,
									ColorTransferWrite& colorTransfer ) 
	{
		auto planes = frameDesc.getPlanes();
		if(planes.size() != 1) {
			throw Exception("Multiplanar formats are not supported");
		}

		//Try to optimize the swizzle
		const auto& supportedFormats = getVulkanFormatSupport(vulkan);
		optimizeSwizzle(planes, supportedFormats);
		if(planes.front().getSwizzle() != vk::ComponentMapping()) {
			throw Exception("Swizzled formats are not supported");
		}
		
		//Try to optimize the planes
		colorTransfer.optimize(planes, supportedFormats);
		if(!colorTransfer.isPassthough()) {
			throw Exception("Plane descriptor must not feature any color transfer");
		}

		//All ok
		return planes.front();
	}

	static RenderPass createRenderPass(	const Vulkan& vulkan,
										DepthStencilFormat depthStencilFmt,
										const ColorTransferWrite&  colorTransfer,
										const Image::Plane& plane )
	{
		constexpr vk::ImageLayout finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

		return RenderPass(
			vulkan,
			colorTransfer,
			plane,
			depthStencilFmt,
			finalLayout
		);
	}

};





/*
 * TargetFrame::Impl
 */

struct TargetFrame::Impl {
	std::shared_ptr<const Cache>				cache;

	vk::UniqueFramebuffer						framebuffer;
	vk::UniqueFence								renderComplete;

	std::shared_ptr<const CommandBuffer>		commandBuffer;

	Impl(	const Vulkan& vulkan,
			const Image& dstImage,
			std::shared_ptr<const Cache> c )
		: cache(std::move(c))
		, framebuffer(createFramebuffer(vulkan, dstImage, *cache)) //TODO
		, renderComplete(vulkan.createFence(true))
		, commandBuffer()
	{
	}

	~Impl() {
		waitCompletion(cache->getVulkan(), Vulkan::NO_TIMEOUT);
	}



	bool waitCompletion(const Vulkan& vulkan, uint64_t timeo) const {
		return vulkan.waitForFences(*renderComplete, true, timeo);
	}

	void beginRenderPass(	const Vulkan& vulkan,
							vk::CommandBuffer cmd, 
							vk::Rect2D renderArea,
							Utils::BufferView<const vk::ClearValue> clearValues,
							vk::SubpassContents contents ) const noexcept
	{
		const vk::RenderPassBeginInfo beginInfo(
			cache->getRenderPass().get(),
			framebuffer.get(),
			renderArea,
			clearValues.size(), clearValues.data()
		);

		vulkan.beginRenderPass(cmd, beginInfo, contents);
	}

	void endRenderPass(	const Vulkan& vulkan, 
						vk::CommandBuffer cmd) const noexcept 
	{
		vulkan.endRenderPass(cmd);
	}

	void draw(	const Vulkan& vulkan, 
				std::shared_ptr<const CommandBuffer> cmd ) 
	{
		//Wait until the rendering finishes
		waitCompletion(vulkan, Vulkan::NO_TIMEOUT);
		
		commandBuffer = std::move(cmd);
		if(commandBuffer) {
			const std::array commandBuffers = {
				commandBuffer->get()
			};

			const vk::SubmitInfo submitInfo(
				0, nullptr,										//Wait semaphores
				nullptr,										//Pipeline stages
				commandBuffers.size(), commandBuffers.data(),	//Command buffers
				0, nullptr										//Signal semaphores
			);

			//Send it to the queue
			vulkan.resetFences(*renderComplete);
			vulkan.submit(
				vulkan.getGraphicsQueue(),
				submitInfo,
				*renderComplete
			);
		}

	}


	static Frame createFrame(	const Vulkan& vulkan, 
								std::shared_ptr<const Frame::Descriptor> desc,
								std::shared_ptr<const Cache>& cache,
								std::shared_ptr<void> usrPtr )
	{
		//Ensure that the cache exists
		if(!cache) {
			cache = createCache(vulkan, *desc, DepthStencilFormat::NONE);
		}
		assert(cache);

		const vk::ImageUsageFlags usage = 
			vk::ImageUsageFlagBits::eColorAttachment ;

		return Frame(
			vulkan,
			cache->getPlane(),
			usage,
			std::move(desc),
			cache->getFrameCache(),
			std::move(usrPtr)
		);
	}

	static std::shared_ptr<const Cache> createCache(const Vulkan& vulkan, 
													const Frame::Descriptor& frameDesc,
													DepthStencilFormat depthStencilFmt )
	{
		return Utils::makeShared<Cache>(vulkan, frameDesc, depthStencilFmt);
	}

	static const RenderPass& getRenderPass(const Cache& cache) noexcept {
		return cache.getRenderPass();
	}


	static Utils::Discrete<ColorFormat> getSupportedFormats(const Vulkan& vulkan) {
		//TODO cache the result
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

private:
	static vk::UniqueFramebuffer createFramebuffer(	const Vulkan& vulkan,
													const Image& target,
													const Cache& cache )
	{
		return cache.getRenderPass().createFramebuffer(vulkan, target);
	}

};





/*
 * TargetFrame
 */

TargetFrame::TargetFrame(	const Vulkan& vulkan,
							std::shared_ptr<const Descriptor> desc,
							std::shared_ptr<const Cache> cache,
							std::shared_ptr<void> usrPtr  )
	: Frame(Impl::createFrame(vulkan, std::move(desc), cache, std::move(usrPtr)))
	, m_impl({}, vulkan, getImage(), std::move(cache))
{
}

TargetFrame::TargetFrame(TargetFrame&& other) noexcept = default; 

TargetFrame::~TargetFrame() = default; 


TargetFrame& TargetFrame::operator=(TargetFrame&& other) noexcept = default; 

bool TargetFrame::waitCompletion(uint64_t timeo) const {
	return m_impl->waitCompletion(getVulkan(), timeo);
}

void TargetFrame::beginRenderPass(	vk::CommandBuffer cmd, 
									vk::Rect2D renderArea,
									Utils::BufferView<const vk::ClearValue> clearValues,
									vk::SubpassContents contents ) const noexcept
{
	m_impl->beginRenderPass(getVulkan(), cmd, renderArea, clearValues, contents);
}



void TargetFrame::endRenderPass(vk::CommandBuffer cmd) const noexcept {
	m_impl->endRenderPass(getVulkan(), cmd);
}

void TargetFrame::draw(std::shared_ptr<const CommandBuffer> cmd) {
	m_impl->draw(getVulkan(), std::move(cmd));
}



std::shared_ptr<const TargetFrame::Cache> TargetFrame::createCache(	const Vulkan& vulkan, 
																	const Frame::Descriptor& frameDesc,
																	DepthStencilFormat depthStencilFmt  )
{
	return Impl::createCache(vulkan, frameDesc, depthStencilFmt);
}

const RenderPass& TargetFrame::getRenderPass(const Cache& cache) noexcept {
	return Impl::getRenderPass(cache);
}

Utils::Discrete<ColorFormat> TargetFrame::getSupportedFormats(const Vulkan& vulkan) {
	return Impl::getSupportedFormats(vulkan);
}

Utils::Discrete<ColorFormat> TargetFrame::getSupportedSrgbFormats(const Vulkan& vulkan) {
	return Impl::getSupportedSrgbFormats(vulkan);
}

}