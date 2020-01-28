#include <Graphics/Swapchain.h>

#include <Exception.h>

#include <limits>
#include <algorithm>
#include <iostream>
#include <set>

namespace Zuazo::Graphics {

Swapchain::Swapchain(	const Vulkan& vulkan,
						const vk::SurfaceKHR& surface,
						Resolution resolution,
						ColorPrimaries primaries, 
						ColorEncoding encoding, 
						PixelFormat format )
	: m_surface(surface)
	, m_swapchain(createSwapchain(
		vulkan, 
		m_surface,
		toVulkan(resolution), 
		toVulkan(format, primaries, encoding)
	))
	, m_images(getImages(vulkan, *m_swapchain))
	, m_imageViews()
{
}


vk::UniqueSwapchainKHR Swapchain::createSwapchain(	const Vulkan& vulkan, 
													const vk::SurfaceKHR& surface, 
													vk::Extent2D resolution, 
													const vk::SurfaceFormatKHR& format )
{
	const auto& physicalDevice = vulkan.getPhysicalDevice();
	if(!physicalDevice.getSurfaceSupportKHR(0, surface, vulkan.getDispatcher())){
		throw Exception("Surface not suppoted by the physical device");
	}

	const auto formats = physicalDevice.getSurfaceFormatsKHR(surface, vulkan.getDispatcher());
	const auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface, vulkan.getDispatcher());
	const auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface, vulkan.getDispatcher());

	const auto surfaceFormat = getFormat(formats, format);
	const auto presentMode = getPresentMode(presentModes);
	const auto extent = getExtent(capabilities, resolution);
	const auto imageCount = getImageCount(capabilities);
	const auto queueFamilies = getQueueFamilies(vulkan);

	const vk::SwapchainCreateInfoKHR createInfo(
		{},
		surface,
		imageCount,
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
		presentMode,
		true,
		nullptr
	);

	std::cout << "Selected swapchain settings:" << std::endl;
	std::cout << vk::to_string(surfaceFormat.format) << " " << vk::to_string(surfaceFormat.colorSpace) << std::endl;
	std::cout << vk::to_string(presentMode) << std::endl;
	std::cout << extent.width << "x" << extent.height << std::endl;
	std::cout << "Image count: " << imageCount << std::endl;

	return vulkan.getDevice().createSwapchainKHRUnique(createInfo, nullptr, vulkan.getDispatcher());
}

std::vector<vk::Image> Swapchain::getImages(const Vulkan& vulkan, const vk::SwapchainKHR& swapchain){
	return vulkan.getDevice().getSwapchainImagesKHR(swapchain, vulkan.getDispatcher());
}





vk::SurfaceFormatKHR Swapchain::getFormat(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats, const vk::SurfaceFormatKHR& format) {
	if(std::find(surfaceFormats.cbegin(), surfaceFormats.cend(), format) != surfaceFormats.cend()){
		return format;
	}

	throw Exception("No compatible surface format was found");
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
