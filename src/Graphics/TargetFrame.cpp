#include <zuazo/Graphics/TargetFrame.h>

#include <algorithm>

namespace Zuazo::Graphics {

TargetFrame::TargetFrame(	const Vulkan& vulkan,
							std::shared_ptr<const Descriptor> desc,
							std::shared_ptr<const Buffer> colorTransferBuffer,
							std::shared_ptr<const Samplers> samplers,
							Utils::BufferView<const Image::Plane> planes,
							std::shared_ptr<const DepthStencil> depthStencil,
							RenderPass renderPass )
	: Frame(
		vulkan,
		std::move(desc),
		std::move(colorTransferBuffer),
		std::move(samplers),
		planes,
		vk::ImageUsageFlagBits::eColorAttachment )
	, m_depthStencil(std::move(depthStencil))
	, m_renderPass(renderPass)
	, m_framebuffer(createFramebuffer(vulkan, getImage(), m_depthStencil.get(), m_renderPass))
	, m_renderComplete(vulkan.createFence(true))
{
}

TargetFrame::~TargetFrame() {
	getVulkan().waitForFences(*m_renderComplete, true, Vulkan::NO_TIMEOUT);
}



const Framebuffer& TargetFrame::getFramebuffer() const noexcept {
	return m_framebuffer;
}

void TargetFrame::beginRenderPass(	vk::CommandBuffer cmd, 
									vk::Rect2D renderArea,
									Utils::BufferView<const vk::ClearValue> clearValues,
									vk::SubpassContents contents ) const noexcept 
{
	const vk::RenderPassBeginInfo beginInfo(
		m_renderPass.get(),
		m_framebuffer.get(),
		renderArea,
		clearValues.size(), clearValues.data()
	);

	getVulkan().beginRenderPass(cmd, beginInfo, contents);
}

void TargetFrame::endRenderPass(vk::CommandBuffer cmd) const noexcept {
	getVulkan().endRenderPass(cmd);
}



void TargetFrame::draw(std::shared_ptr<const CommandBuffer> cmd) {
	//Wait until the rendering finishes
	getVulkan().waitForFences(*m_renderComplete, true, Vulkan::NO_TIMEOUT);
	
	m_commandBuffer = std::move(cmd);

	assert(m_commandBuffer);
	const std::array commandBuffers = {
		m_commandBuffer->get()
	};

	const vk::SubmitInfo submitInfo(
		0, nullptr,										//Wait semaphores
		nullptr,										//Pipeline stages
		commandBuffers.size(), commandBuffers.data(),	//Command buffers
		0, nullptr										//Signal semaphores
	);


	//Send it to the queue
	getVulkan().resetFences(*m_renderComplete);
	getVulkan().submit(
		getVulkan().getGraphicsQueue(),
		submitInfo,
		*m_renderComplete
	);
}



Framebuffer TargetFrame::createFramebuffer(	const Vulkan& vulkan,
											const Image& image,
											const DepthStencil* depthStencil,
											RenderPass renderPass )
{
	return Framebuffer(
		vulkan,
		image,
		nullptr,
		depthStencil,
		renderPass
	);
}

}