#pragma once

#include "../ZuazoBase.h"
#include "../Video.h"
#include "../Utils/Pimpl.h"
#include "../Signal/Input.h"

#include <tuple>

namespace Zuazo::Outputs{

class Window final
	: public ZuazoBase
{
public:
	Window(Instance& instance, const std::string& name);
	Window(Instance& instance, std::string&& name);
	Window(const Window& other) = delete;
	Window(Window&& other);
	virtual ~Window();

	Window&					operator=(const Window& other) = delete;
	Window&					operator=(Window&& other);

	virtual void 			open()  final;
	virtual void 			close() final;
	
	virtual void 			setVideoMode(const VideoMode& videoMode) final;

	inline static const auto PADS = std::make_tuple(Signal::Input<Video>("videoIn0"));
private:
	struct Impl;
	Utils::Pimpl<Impl>		m_impl;











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
	static vk::PipelineLayout					createPipelineLayout(const Graphics::Vulkan& vulkan);
	static vk::UniquePipeline 					createPipeline(	const Graphics::Vulkan& vulkan,
																const Graphics::Frame::Geometry& geom,
																vk::RenderPass renderPass,
																vk::PipelineLayout layout,
																const vk::Extent2D& extent );	
	static std::vector<vk::UniqueFramebuffer> 	createFramebuffers(	const Graphics::Vulkan& vulkan,
																	vk::RenderPass renderPass,
																	const std::vector<vk::UniqueImageView>& imageViews,
																	const vk::Extent2D& extent );
	static vk::UniqueCommandPool				createCommandPool(const Graphics::Vulkan& vulkan);
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