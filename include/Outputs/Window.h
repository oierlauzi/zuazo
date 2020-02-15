#pragma once

#include "../ZuazoBase.h"
#include "../Graphics/GLFW.h"
#include "../Graphics/Vulkan.h"

#include <memory>

namespace Zuazo::Outputs{

class Window 
	: public ZuazoBase
{
public:
	using ZuazoBase::ZuazoBase;
	using ZuazoBase::operator=;

	virtual void 							open() override;
	virtual void 							close() override;
	
	virtual void 							setVideoMode(const VideoMode& videoMode) override;
private:
	struct Implementation {
		Graphics::GLFW::Window					window;
		vk::UniqueSurfaceKHR					surface;
		vk::UniqueSwapchainKHR					swapchain;
		std::vector<vk::UniqueImageView>		swapchainImageViews;
		vk::UniqueRenderPass					renderPass;
		vk::UniquePipelineLayout 				pipelineLayout;
		vk::UniquePipeline						pipeline;
		std::vector<vk::UniqueFramebuffer>		framebuffers;
	};

	std::unique_ptr<Implementation>			m_implementation;


	static vk::UniqueSwapchainKHR			createSwapchain(const Graphics::Vulkan& vulkan, 
														const vk::SurfaceKHR& surface, 
														const vk::Extent2D& extent, 
														const vk::SurfaceFormatKHR& surfaceFormat,
														vk::SwapchainKHR old = {} );
	static std::vector<vk::UniqueImageView> createImageViews(	const Graphics::Vulkan& vulkan,
																vk::SwapchainKHR swapchain,
																vk::Format format );
	static vk::UniqueRenderPass 			createRenderPass(	const Graphics::Vulkan& vulkan,
																vk::Format format );
	static vk::UniquePipelineLayout			createPipelineLayout(const Graphics::Vulkan& vulkan);
	static vk::UniquePipeline 				createPipeline(		const Graphics::Vulkan& vulkan,
																vk::RenderPass renderPass,
																vk::PipelineLayout layout,
																const vk::Extent2D& extent );	
	static std::vector<vk::UniqueFramebuffer> createFramebuffers(	const Graphics::Vulkan& vulkan,
																	vk::RenderPass renderPass,
																	const std::vector<vk::UniqueImageView>& imageViews,
																	const vk::Extent2D& extent );

	static vk::PresentModeKHR				getPresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
	static uint32_t							getImageCount(const vk::SurfaceCapabilitiesKHR& cap);
	static std::vector<uint32_t>			getQueueFamilies(const Graphics::Vulkan& vulkan);
};

}