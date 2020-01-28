#pragma once

#include "Vulkan.h"
#include "Window.h"
#include "VulkanConversions.h"
#include "../PixelProperties.h"

namespace Zuazo::Graphics {

class Swapchain {
public:
	Swapchain() = default;
	Swapchain	(const Vulkan& vulkan, 
				const vk::SurfaceKHR& surface,
				Resolution resolution,
				ColorPrimaries primaries, 
				ColorEncoding encoding, 
				PixelFormat format );
	Swapchain(const Swapchain& other) = delete;
	Swapchain(Swapchain&& other) = default;
	~Swapchain() = default;

	Swapchain& 								operator=(const Swapchain& other) = default;
	Swapchain& 								operator=(Swapchain&& other) = default;

private:
	vk::SurfaceKHR							m_surface;
	vk::UniqueSwapchainKHR					m_swapchain;
	std::vector<vk::Image>					m_images;
	std::vector<vk::UniqueImageView>		m_imageViews;

	static vk::UniqueSwapchainKHR			createSwapchain(const Vulkan& vulkan, 
															const vk::SurfaceKHR& surface, 
															vk::Extent2D resolution, 
															const vk::SurfaceFormatKHR& format );
	static std::vector<vk::Image>			getImages(const Vulkan& vulkan, const vk::SwapchainKHR& swapchain);
	static std::vector<vk::UniqueImageView>	getImageViews();
	
	static vk::SurfaceFormatKHR				getFormat(	const std::vector<vk::SurfaceFormatKHR>& surfaceFormats, 
														const vk::SurfaceFormatKHR& format );
	static vk::PresentModeKHR				getPresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
	static vk::Extent2D						getExtent(const vk::SurfaceCapabilitiesKHR& cap, vk::Extent2D windowExtent);
	static uint32_t							getImageCount(const vk::SurfaceCapabilitiesKHR& cap);
	static std::vector<uint32_t>			getQueueFamilies(const Vulkan& vulkan);

};

}