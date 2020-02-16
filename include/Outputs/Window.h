#pragma once

#include "../ZuazoBase.h"
#include "../Graphics/GLFW.h"
#include "../Graphics/Vulkan.h"
#include "../Signal/Input.h"

#include <memory>

namespace Zuazo::Outputs{

class Window 
	: public ZuazoBase
{
public:
	using ZuazoBase::ZuazoBase;
	using ZuazoBase::operator=;

	virtual void 								open() override;
	virtual void 								close() override;
	
	virtual void 								setVideoMode(const VideoMode& videoMode) override;
private:
	struct Implementation {
		Graphics::GLFW::Window						window;
		vk::UniqueSurfaceKHR						surface;
		vk::UniqueSwapchainKHR						swapchain;
		std::vector<vk::UniqueImageView>			swapchainImageViews;
		vk::UniqueRenderPass						renderPass;
		vk::UniquePipelineLayout 					pipelineLayout;
		vk::UniquePipeline							pipeline;
		std::vector<vk::UniqueFramebuffer>			framebuffers;
		vk::UniqueCommandPool						commandPool;
		std::vector<vk::UniqueCommandBuffer>		commandBuffers;
		vk::UniqueSemaphore 						imageAvailableSemaphore;
		vk::UniqueSemaphore							renderFinishedSemaphore;
		vk::UniqueFence								renderFinishedFence;
	};
		
	static constexpr auto NO_TIMEOUT = std::numeric_limits<uint64_t>::max();

	mutable std::mutex							m_resizeMutex;//TODO only for testing

	Signal::Input<int>							m_input = Signal::Input<int>("videoIn0", this); //TODO modify to be frames
	std::unique_ptr<Implementation>				m_implementation;

	std::tuple<vk::Extent2D, vk::SurfaceFormatKHR> getVulkanVideoMode() const;
	void										recreate();
	void										drawFrameProvisional(); //TODO only for testing


	static vk::UniqueSwapchainKHR				createSwapchain(const Graphics::Vulkan& vulkan, 
																const vk::SurfaceKHR& surface, 
																const vk::Extent2D& extent, 
																const vk::SurfaceFormatKHR& surfaceFormat,
																const vk::SurfaceCapabilitiesKHR& capabilities,
																vk::SwapchainKHR old = {} );
	static std::vector<vk::UniqueImageView> 	createImageViews(	const Graphics::Vulkan& vulkan,
																	vk::SwapchainKHR swapchain,
																	vk::Format format );
	static vk::UniqueRenderPass 				createRenderPass(	const Graphics::Vulkan& vulkan,
																	vk::Format format );
	static vk::UniquePipelineLayout				createPipelineLayout(const Graphics::Vulkan& vulkan);
	static vk::UniquePipeline 					createPipeline(	const Graphics::Vulkan& vulkan,
																vk::RenderPass renderPass,
																vk::PipelineLayout layout,
																const vk::Extent2D& extent );	
	static std::vector<vk::UniqueFramebuffer> 	createFramebuffers(	const Graphics::Vulkan& vulkan,
																	vk::RenderPass renderPass,
																	const std::vector<vk::UniqueImageView>& imageViews,
																	const vk::Extent2D& extent );
	static vk::UniqueCommandPool				createCommandPool(	const Graphics::Vulkan& vulkan,
																	uint32_t queueIndex );
	static std::vector<vk::UniqueCommandBuffer>	createCommandBuffers(	const Graphics::Vulkan& vulkan,
																		vk::CommandPool pool,
																		uint32_t count );

	static vk::Extent2D							getExtent(	const vk::SurfaceCapabilitiesKHR& cap, 
															const vk::Extent2D& windowExtent );
	static vk::SurfaceFormatKHR					getSurfaceFormat(	const std::vector<vk::SurfaceFormatKHR>& formats,
																	const vk::SurfaceFormatKHR& desired );
	static uint32_t								getImageCount(const vk::SurfaceCapabilitiesKHR& cap);
	static vk::PresentModeKHR					getPresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
	static std::vector<uint32_t>				getQueueFamilies(const Graphics::Vulkan& vulkan);
};

}