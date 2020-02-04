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
						vk::SwapchainKHR old )
	: m_swapchain(createSwapchain(
		vulkan, 
		surface,
		extent, 
		surfaceFormat,
		old
	))
	, m_images(getImages(vulkan, *m_swapchain))
	, m_imageViews(getImageViews(
		vulkan, 
		surfaceFormat.format, 
		m_images
	))
{
}


const vk::SwapchainKHR&	Swapchain::getSwapchain() const {
	return *m_swapchain;
}

vk::SwapchainKHR Swapchain::getSwapchain() {
	return *m_swapchain;
}

const std::vector<vk::UniqueImageView>& Swapchain::getImageViews() const{
	return m_imageViews;
}

std::vector<vk::UniqueImageView>& Swapchain::getImageViews() {
	return m_imageViews;
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

	const auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface, vulkan.getDispatcher());
	const auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface, vulkan.getDispatcher());
	
	const auto queueFamilies = getQueueFamilies(vulkan);

	const vk::SwapchainCreateInfoKHR createInfo(
		{},
		surface,
		getImageCount(capabilities),
		surfaceFormat.format,
		surfaceFormat.colorSpace,
		getExtent(capabilities, extent),
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

std::vector<vk::UniqueImageView> Swapchain::getImageViews(	const Vulkan& vulkan,
															vk::Format format, 
															const std::vector<vk::Image>& images ) 
{
	std::vector<vk::UniqueImageView> result;
	result.reserve(images.size());

	for(const auto& image : images){
		vk::ImageViewCreateInfo createInfo(
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

		auto imageView = vulkan.getDevice().createImageViewUnique(createInfo, nullptr, vulkan.getDispatcher());
		result.emplace_back(std::move(imageView));
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

vk::Extent2D Swapchain::getExtent(const vk::SurfaceCapabilitiesKHR& cap, vk::Extent2D windowExtent){
	constexpr uint32_t MAX_EXTENT = std::numeric_limits<uint32_t>::max();

	if(cap.currentExtent.width == MAX_EXTENT || cap.currentExtent.height == MAX_EXTENT){
		return vk::Extent2D(
			std::max(cap.minImageExtent.width, std::min(cap.maxImageExtent.width, windowExtent.width)),
			std::max(cap.minImageExtent.height, std::min(cap.maxImageExtent.height, windowExtent.height))
		);
	} else {
		return cap.currentExtent;
	}
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
