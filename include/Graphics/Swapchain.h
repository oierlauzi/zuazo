#pragma once

#include "Vulkan.h"
#include "Window.h"
#include "VulkanConversions.h"

namespace Zuazo::Graphics {

class Swapchain {
public:
	Swapchain() = default;
	Swapchain(	const Vulkan& vulkan, 
				const vk::SurfaceKHR& surface,
				const vk::Extent2D& extent,
				const vk::SurfaceFormatKHR& format,
				vk::SwapchainKHR old = {});
	Swapchain(const Swapchain& other) = delete;
	Swapchain(Swapchain&& other) = default;
	~Swapchain() = default;

	Swapchain& 								operator=(const Swapchain& other) = delete;
	Swapchain& 								operator=(Swapchain&& other) = default;

	const vk::SwapchainKHR&					getSwapchain() const;
	vk::SwapchainKHR						getSwapchain();
	const std::vector<vk::UniqueImageView>&	getImageViews() const;
	std::vector<vk::UniqueImageView>&		getImageViews();
private:
	vk::UniqueSwapchainKHR					m_swapchain;
	std::vector<vk::Image>					m_images;
	std::vector<vk::UniqueImageView>		m_imageViews;

	static vk::UniqueSwapchainKHR			createSwapchain(const Vulkan& vulkan, 
															const vk::SurfaceKHR& surface, 
															const vk::Extent2D& extent, 
															const vk::SurfaceFormatKHR& format,
															vk::SwapchainKHR old );
	static std::vector<vk::Image>			getImages(const Vulkan& vulkan, const vk::SwapchainKHR& swapchain);
	static std::vector<vk::UniqueImageView>	getImageViews(	const Vulkan& vulkan,
															vk::Format format, 
															const std::vector<vk::Image>& images );
	
	static vk::PresentModeKHR				getPresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
	static vk::Extent2D						getExtent(const vk::SurfaceCapabilitiesKHR& cap, vk::Extent2D windowExtent);
	static uint32_t							getImageCount(const vk::SurfaceCapabilitiesKHR& cap);
	static std::vector<uint32_t>			getQueueFamilies(const Vulkan& vulkan);

};

}