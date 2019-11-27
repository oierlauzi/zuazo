#include <Graphics/Surface.h>

#include <Exception.h>

#include <limits>
#include <algorithm>
#include <iostream>
#include <set>

namespace Zuazo::Graphics {

Surface::Surface(const Vulkan& vulkan, const Window& window, bool vSync)
	: m_surface(window.getSurface(vulkan.getInstance()))
	, m_swapchain(createSwapchain(vulkan, *m_surface, static_cast<vk::Extent2D>(window.getResolution()), vSync))
	, m_images(getImages(vulkan, *m_swapchain))
{

}



vk::UniqueSwapchainKHR Surface::createSwapchain(const Vulkan& vulkan, 
												const vk::SurfaceKHR& surface, 
												vk::Extent2D resolution, 
												bool vSync )
{
	const auto& physicalDevice = vulkan.getPhysicalDevice();
	if(!physicalDevice.getSurfaceSupportKHR(0, surface)){
		throw Exception("Surface not suppoted by the physical device");
	}

	const auto formats = physicalDevice.getSurfaceFormatsKHR(surface);
	const auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
	const auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

	const auto format = getFormat(formats);
	const auto presentMode = getPresentMode(presentModes, vSync);
	const auto extent = getExtent(capabilities, resolution);
	const uint32_t imageCount = getImageCount(capabilities);
	const auto queueFamilies = getQueueFamilies(vulkan);

	const vk::SwapchainCreateInfoKHR createInfo(
		{},
		surface,
		imageCount,
		format.format,
		format.colorSpace,
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
	std::cout << vk::to_string(format.format) << " " << vk::to_string(format.colorSpace) << std::endl;
	std::cout << vk::to_string(presentMode) << std::endl;
	std::cout << extent.width << "x" << extent.height << std::endl;
	std::cout << "Image count: " << imageCount << std::endl;

	return vulkan.getDevice().createSwapchainKHRUnique(createInfo);
}

std::vector<vk::Image> Surface::getImages(const Vulkan& vulkan, const vk::SwapchainKHR& swapchain){
	return vulkan.getDevice().getSwapchainImagesKHR(swapchain);
}





vk::SurfaceFormatKHR Surface::getFormat(const std::vector<vk::SurfaceFormatKHR>& formats) {
	vk::SurfaceFormatKHR desired; 
	desired.format = vk::Format::eB8G8R8A8Unorm;
	desired.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;

	if(std::find(formats.cbegin(), formats.cend(), desired) != formats.cend()){
		return desired;
	}

	throw Exception("No compatible surface format was found");
}

vk::PresentModeKHR Surface::getPresentMode(const std::vector<vk::PresentModeKHR>& presentModes, bool vSync) {
	std::vector<vk::PresentModeKHR> desiredModes;

	if(vSync){
		desiredModes = {
			vk::PresentModeKHR::eMailbox,
			vk::PresentModeKHR::eFifo
		};
	} else {
		desiredModes = {
			vk::PresentModeKHR::eFifoRelaxed,
			vk::PresentModeKHR::eImmediate
		};
	}

	for(const auto& desired : desiredModes){
		if(std::find(presentModes.cbegin(), presentModes.cend(), desired) != presentModes.cend()){
			return desired;
		}
	}

	std::string errMsg = "No present modes compatible with vSync=";
	if(vSync){
		errMsg+="true";
	}else{
		errMsg+="false";
	}
	errMsg+=" were found";

	throw Exception(errMsg);
}

vk::Extent2D Surface::getExtent(const vk::SurfaceCapabilitiesKHR& cap, vk::Extent2D windowExtent){
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

uint32_t Surface::getImageCount(const vk::SurfaceCapabilitiesKHR& cap){
	const uint32_t desired = cap.minImageCount + 1;

	if(cap.maxImageCount){
		return std::min(desired, cap.maxImageCount);
	} else {
		return desired;
	}
}

std::vector<uint32_t> Surface::getQueueFamilies(const Vulkan& vulkan){
	const std::set<uint32_t> families = {
		vulkan.getGraphicsQueueIndex(),
		vulkan.getPresentationQueueIndex()
	};

	return std::vector<uint32_t>(families.cbegin(), families.cend());
}

}
