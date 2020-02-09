#include <Graphics/Swapchain.h>

#include <Exception.h>

#include <limits>
#include <algorithm>
#include <iostream>
#include <set>

namespace Zuazo::Graphics {

Swapchain::Swapchain(	const Vulkan& vulkan,
						const vk::SurfaceKHR& surface,
						const vk::Extent2D& extent,
						const vk::SurfaceFormatKHR& surfaceFormat,
						vk::RenderPass renderPass,
						vk::SwapchainKHR old )
	: m_swapchain(createSwapchain( vulkan, 
		surface,
		extent, 
		surfaceFormat,
		old ))
	, m_images(getImages(vulkan, *m_swapchain))
	, m_imageViews(createImageViews( vulkan, 
		surfaceFormat.format, 
		m_images ))
	, m_framebuffers(createFramebuffers( vulkan,
		extent,
		renderPass,
		m_imageViews
	))
{
}


const vk::SwapchainKHR&	Swapchain::getSwapchain() const {
	return *m_swapchain;
}

vk::SwapchainKHR Swapchain::getSwapchain() {
	return *m_swapchain;
}

const std::vector<vk::UniqueFramebuffer>& Swapchain::getFramebuffers() const{
	return m_framebuffers;
}



vk::UniqueSwapchainKHR Swapchain::createSwapchain(	const Vulkan& vulkan, 
													const vk::SurfaceKHR& surface, 
													const vk::Extent2D& extent, 
													const vk::SurfaceFormatKHR& surfaceFormat,
													vk::SwapchainKHR old )
{
	const auto& physicalDevice = vulkan.getPhysicalDevice();
	if(!physicalDevice.getSurfaceSupportKHR(0, surface, vulkan.getDispatcher())){
		throw Exception("Surface not suppoted by the physical device");
	}

	const auto formats = physicalDevice.getSurfaceFormatsKHR(surface, vulkan.getDispatcher());
	if(std::find(formats.cbegin(), formats.cend(), surfaceFormat) == formats.cend()){
		throw Exception("Surface format is not supported");
	}

	const auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface, vulkan.getDispatcher());
	const auto queueFamilies = getQueueFamilies(vulkan);
	const auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface, vulkan.getDispatcher());

	if(	extent.width < capabilities.minImageExtent.width || 
		extent.height < capabilities.minImageExtent.height || 
		extent.width > capabilities.maxImageExtent.width || 
		extent.height > capabilities.maxImageExtent.height )
	{
		throw Exception("Surface extent is not supported");
	}

	const vk::SwapchainCreateInfoKHR createInfo(
		{},
		surface,
		getImageCount(capabilities),
		surfaceFormat.format,
		surfaceFormat.colorSpace,
		extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		(queueFamilies.size() > 1) ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
		queueFamilies.size(),
		queueFamilies.data(),
		capabilities.currentTransform,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		getPresentMode(presentModes),
		true,
		old
	);

	return vulkan.getDevice().createSwapchainKHRUnique(createInfo, nullptr, vulkan.getDispatcher());
}

std::vector<vk::Image> Swapchain::getImages(const Vulkan& vulkan, const vk::SwapchainKHR& swapchain){
	return vulkan.getDevice().getSwapchainImagesKHR(swapchain, vulkan.getDispatcher());
}

std::vector<vk::UniqueImageView> Swapchain::createImageViews(	const Vulkan& vulkan,
																vk::Format format, 
																const std::vector<vk::Image>& images ) 
{
	std::vector<vk::UniqueImageView> result;
	result.reserve(images.size());

	for(const auto& image : images){
		const vk::ImageViewCreateInfo createInfo(
			{},
			image,
			vk::ImageViewType::e2D,
			format,
			{},
			vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor,
				0, 1, 0, 1
			)
		);

		auto iv = vulkan.getDevice().createImageViewUnique(createInfo, nullptr, vulkan.getDispatcher());
		result.emplace_back(std::move(iv));
	}

	return result;
}

std::vector<vk::UniqueFramebuffer> Swapchain::createFramebuffers(	const Vulkan& vulkan,
																	const vk::Extent2D& extent,
																	vk::RenderPass renderPass,
																	const std::vector<vk::UniqueImageView>& imageViews )
{
	std::vector<vk::UniqueFramebuffer> result;
	result.reserve(imageViews.size());

	for(const auto& img : imageViews){
		const std::array attachments = {
			*img
		};

		const vk::FramebufferCreateInfo createInfo(
			{},
			renderPass,
			attachments.size(),
			attachments.data(),
			extent.width, extent.height,
			1
		);

		auto fb = vulkan.getDevice().createFramebufferUnique(createInfo, nullptr, vulkan.getDispatcher());
		result.emplace_back(std::move(fb));
	}

	return result;
}





vk::PresentModeKHR Swapchain::getPresentMode(const std::vector<vk::PresentModeKHR>& presentModes) {
	const std::vector<vk::PresentModeKHR> prefered = {
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

uint32_t Swapchain::getImageCount(const vk::SurfaceCapabilitiesKHR& cap){
	const uint32_t desired = cap.minImageCount + 1;

	if(cap.maxImageCount){
		return std::min(desired, cap.maxImageCount);
	} else {
		return desired;
	}
}

std::vector<uint32_t> Swapchain::getQueueFamilies(const Vulkan& vulkan){
	const std::set<uint32_t> families = {
		vulkan.getGraphicsQueueIndex(),
		vulkan.getPresentationQueueIndex()
	};

	return std::vector<uint32_t>(families.cbegin(), families.cend());
}

}
