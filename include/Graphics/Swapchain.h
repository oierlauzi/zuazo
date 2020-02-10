#pragma once

#include "Vulkan.h"
#include "VulkanConversions.h"

namespace Zuazo::Graphics {

class Swapchain {
public:
	Swapchain() = default;
	Swapchain(	const Vulkan& vulkan, 
				const vk::SurfaceKHR& surface,
				const vk::Extent2D& extent,
				const vk::SurfaceFormatKHR& format,
				vk::RenderPass renderPass,
				vk::SwapchainKHR old = {});
	Swapchain(const Swapchain& other) = delete;
	Swapchain(Swapchain&& other) = default;
	~Swapchain() = default;

	Swapchain& 								operator=(const Swapchain& other) = delete;
	Swapchain& 								operator=(Swapchain&& other) = default;

	vk::SwapchainKHR						getSwapchain() const;

	const std::vector<vk::UniqueFramebuffer>& getFramebuffers() const;
private:
	vk::UniqueSwapchainKHR					m_swapchain;
	std::vector<vk::Image>					m_images;
	std::vector<vk::UniqueImageView>		m_imageViews;
	std::vector<vk::UniqueFramebuffer>		m_framebuffers;

	static vk::UniqueSwapchainKHR			createSwapchain(const Vulkan& vulkan, 
															const vk::SurfaceKHR& surface, 
															const vk::Extent2D& extent, 
															const vk::SurfaceFormatKHR& format,
															vk::SwapchainKHR old );
	static std::vector<vk::Image>			getImages(const Vulkan& vulkan, const vk::SwapchainKHR& swapchain);
	static std::vector<vk::UniqueImageView>	createImageViews(	const Vulkan& vulkan,
																vk::Format format, 
																const std::vector<vk::Image>& images );
	static std::vector<vk::UniqueFramebuffer> createFramebuffers(	const Vulkan& vulkan,
																	const vk::Extent2D& extent,
																	vk::RenderPass renderPass,
																	const std::vector<vk::UniqueImageView>& imageViews );

	
	static vk::PresentModeKHR				getPresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
	static uint32_t							getImageCount(const vk::SurfaceCapabilitiesKHR& cap);
	static std::vector<uint32_t>			getQueueFamilies(const Vulkan& vulkan);

};

}