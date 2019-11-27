#pragma once

#include "Vulkan.h"
#include "Window.h"

namespace Zuazo::Graphics {

class Surface {
public:
	Surface() = default;
	Surface(const Vulkan& vulkan, const Window& window, bool vSync);
	Surface(Surface&& other) = default;
	~Surface() = default;

	Surface& 								operator=(Surface&& other) = default;

private:
	vk::UniqueSurfaceKHR					m_surface;
	vk::UniqueSwapchainKHR					m_swapchain;
	std::vector<vk::Image>					m_images;
	std::vector<vk::UniqueImageView>		m_imageViews;

	static vk::UniqueSwapchainKHR			createSwapchain(const Vulkan& vulkan, const vk::SurfaceKHR& surface, vk::Extent2D resolution, bool vSync);
	static std::vector<vk::Image>			getImages(const Vulkan& vulkan, const vk::SwapchainKHR& swapchain);
	static std::vector<vk::UniqueImageView>	getImageViews();
	
	static vk::SurfaceFormatKHR				getFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
	static vk::PresentModeKHR				getPresentMode(const std::vector<vk::PresentModeKHR>& presentModes, bool vSync);
	static vk::Extent2D						getExtent(const vk::SurfaceCapabilitiesKHR& cap, vk::Extent2D windowExtent);
	static uint32_t							getImageCount(const vk::SurfaceCapabilitiesKHR& cap);
	static std::vector<uint32_t>			getQueueFamilies(const Vulkan& vulkan);

};

}