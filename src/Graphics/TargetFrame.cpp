#include <zuazo/Graphics/TargetFrame.h>

#include <algorithm>

namespace Zuazo::Graphics {

TargetFrame::TargetFrame(	const Vulkan& vulkan,
							std::shared_ptr<const Descriptor> desc,
							std::shared_ptr<const Buffer> colorTransfer,
							Utils::BufferView<const Frame::PlaneDescriptor> planes,
							std::shared_ptr<const DepthStencil> depthStencil,
							RenderPass renderPass )
	: Frame(
		vulkan,
		std::move(desc),
		std::move(colorTransfer),
		planes,
		vk::ImageUsageFlagBits::eColorAttachment )
	, m_depthStencil(std::move(depthStencil))
	, m_renderPass(renderPass)
	, m_framebuffer(createFramebuffer(vulkan, planes, m_depthStencil, m_renderPass.getRenderPass(), getImageViews()))
	, m_renderComplete(vulkan.createFence(true))
{
}

TargetFrame::~TargetFrame() {
	getVulkan().waitForFences(*m_renderComplete, true, Vulkan::NO_TIMEOUT);
}



vk::Framebuffer TargetFrame::getFramebuffer() const noexcept {
	return *m_framebuffer;
}

void TargetFrame::beginRenderPass(	vk::CommandBuffer cmd, 
									vk::Rect2D renderArea,
									Utils::BufferView<const vk::ClearValue> clearValues,
									vk::SubpassContents contents ) const noexcept 
{
	const vk::RenderPassBeginInfo beginInfo(
		m_renderPass.getRenderPass(),
		*m_framebuffer,
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
		m_commandBuffer->getCommandBuffer()
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



vk::UniqueFramebuffer TargetFrame::createFramebuffer(	const Graphics::Vulkan& vulkan,
														Utils::BufferView<const PlaneDescriptor> planes,
														const std::shared_ptr<const DepthStencil>& depthStencil,
														vk::RenderPass renderPass,
														const std::vector<vk::UniqueImageView>& imageViews )
{
	assert(renderPass);
	assert(imageViews.size() == planes.size());
	const auto attachmentCount = imageViews.size() + (depthStencil ? 1 : 0);

	//Enumerate all attachments
	std::vector<vk::ImageView> attachments;
	attachments.reserve(attachmentCount);
	std::transform(
		imageViews.cbegin(), imageViews.cend(),
		std::back_inserter(attachments),
		[] (const vk::UniqueImageView& iv) -> vk::ImageView {
			return *iv;
		}
	);

	if(depthStencil) {
		attachments.emplace_back(depthStencil->getImageView());
	}

	assert(attachments.size() == attachmentCount);

	const vk::FramebufferCreateInfo createInfo(
		{},
		renderPass,
		attachments.size(),
		attachments.data(),
		planes[0].extent.width, planes[0].extent.height,
		1
	);
	
	//Ensure that all planes have the same size
	for(size_t i = 1; i < planes.size(); ++i) {
		assert(planes[i].extent.width >= createInfo.width); 
		assert(planes[i].extent.height >= createInfo.height); 
	}

	return vulkan.createFramebuffer(createInfo);
}

}