#include <zuazo/Graphics/TargetFrame.h>

#include <algorithm>

namespace Zuazo::Graphics {

TargetFrame::TargetFrame(	const Vulkan& vulkan,
							std::shared_ptr<const Descriptor> desc,
							std::shared_ptr<const Buffer> colorTransfer,
							Utils::BufferView<const Frame::PlaneDescriptor> planes,
							std::shared_ptr<const vk::UniqueRenderPass> renderPass,
							std::shared_ptr<const DepthStencil> depthStencil )
	: Frame(
		vulkan,
		std::move(desc),
		std::move(colorTransfer),
		planes,
		vk::ImageUsageFlagBits::eColorAttachment )
	, m_renderPass(std::move(renderPass))
	, m_depthStencil(std::move(depthStencil))
	, m_framebuffer(createFramebuffer(vulkan, planes, m_renderPass, m_depthStencil, getImageViews()))
	, m_renderComplete(vulkan.createFence(true))
{
}

TargetFrame::~TargetFrame() {
	waitDependecies();
}



vk::Framebuffer TargetFrame::getFramebuffer() const {
	return *m_framebuffer;
}

void TargetFrame::beginRenderPass(	vk::CommandBuffer cmd, 
									vk::Rect2D renderArea,
									Utils::BufferView<const vk::ClearValue>& clearValues,
									vk::SubpassContents contents ) const 
{
	const vk::RenderPassBeginInfo beginInfo(
		**m_renderPass,
		*m_framebuffer,
		renderArea,
		clearValues.size(), clearValues.data()
	);

	getVulkan().beginRenderPass(cmd, beginInfo, contents);
}

void TargetFrame::endRenderPass(vk::CommandBuffer cmd) const {
	getVulkan().endRenderPass(cmd);
}



void TargetFrame::draw(std::shared_ptr<const CommandBuffer> cmd) {
	waitDependecies(); //Wait until rendering finishes
	m_commandBuffer = std::move(cmd);
	assert(m_commandBuffer);

	const std::array commandBuffers = {
		m_commandBuffer->getCommandBuffer()
	};

	const vk::SubmitInfo submitInfo(
		0, nullptr,										//Wait semaphores
		nullptr,										//Pipeline stages //TODO
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

	addDependecy(*m_renderComplete);
}



vk::UniqueFramebuffer TargetFrame::createFramebuffer(	const Graphics::Vulkan& vulkan,
														Utils::BufferView<const PlaneDescriptor> planes,
														const std::shared_ptr<const vk::UniqueRenderPass>& renderPass,
														const std::shared_ptr<const DepthStencil>& depthStencil,
														const std::vector<vk::UniqueImageView>& imageViews )
{
	assert(renderPass);
	assert(*renderPass);
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
		**renderPass,
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



/*
 * TargetFrame::RenderPass
 */
/*

vk::UniqueRenderPass TargetFrame::RenderPass::createRenderPass(	const Vulkan& vulkan,
																Utils::BufferView<const PlaneDescriptor> planes, 
																vk::Format depthStencilFmt )
{
	const bool hasDepthStencil = hasDepth(depthStencilFmt) || hasStencil(depthStencilFmt);
	const size_t attachmentCount = planes.size() + (hasDepthStencil ? 1 : 0);
	assert(hasDepthStencil || depthStencilFmt == vk::Format::eUndefined);

	//Add all the color attachments
	std::vector<vk::AttachmentDescription> attachments;
	attachments.reserve(attachmentCount);

	for(const auto& plane : planes) {
		attachments.emplace_back(
			vk::AttachmentDescriptionFlags(),			//Flags
			plane.format,								//Attachemnt format
			vk::SampleCountFlagBits::e1,				//Sample count
			vk::AttachmentLoadOp::eClear,				//Color attachment load operation
			vk::AttachmentStoreOp::eStore,				//Color attachemnt store operation
			vk::AttachmentLoadOp::eDontCare,			//Stencil attachment load operation
			vk::AttachmentStoreOp::eDontCare,			//Stencil attachment store operation
			vk::ImageLayout::eUndefined,				//Initial layout
			vk::ImageLayout::eShaderReadOnlyOptimal		//Final layout
		);
	}

	if(hasDepthStencil) {
		attachments.emplace_back(
			vk::AttachmentDescriptionFlags(),			//Flags
			depthStencilFmt,							//Attachemnt format
			vk::SampleCountFlagBits::e1,				//Sample count
			vk::AttachmentLoadOp::eDontCare,			//Color attachment load operation
			vk::AttachmentStoreOp::eDontCare,			//Color attachemnt store operation
			vk::AttachmentLoadOp::eClear,				//Stencil attachment load operation
			vk::AttachmentStoreOp::eStore,				//Stencil attachment store operation
			vk::ImageLayout::eUndefined,				//Initial layout
			vk::ImageLayout::eUndefined					//Final layout
		);
	}
	assert(attachments.size() == attachmentCount);

	//Add all the color attachment references
	std::vector<vk::AttachmentReference> colorAttachmentReferences;
	attachments.reserve(planes.size());

	for(size_t i = 0; i < planes.size(); ++i) {
		colorAttachmentReferences.emplace_back(
			colorAttachmentReferences.size(), 				//Attachments index
			vk::ImageLayout::eColorAttachmentOptimal 		//Attachemnt layout
		);
	}

	//Add the depth/stencil attachment references
	const vk::AttachmentReference depthStencilAttachmentReference(
		colorAttachmentReferences.size(),					//The one after color attachments
		vk::ImageLayout::eDepthStencilAttachmentOptimal		//Attachemnt layout
	);

	//Create a subpass with all attachments
	const std::array subpasses = {
		vk::SubpassDescription(
			{},																	//Flags
			vk::PipelineBindPoint::eGraphics,									//Pipeline bind point
			0, nullptr,															//Input attachments
			colorAttachmentReferences.size(), colorAttachmentReferences.data(),	//Color attachments
			nullptr,															//Resolve attachemnts
			hasDepthStencil ? &depthStencilAttachmentReference : nullptr,		//Depth / Stencil attachemnts
			0, nullptr															//Preserve attachments
		)
	};

	//Use only external dependencies
	constexpr vk::AccessFlags destinationAccessFlags =
		vk::AccessFlagBits::eColorAttachmentRead |
		vk::AccessFlagBits::eColorAttachmentWrite;

	constexpr std::array subpassDependencies = {
		vk::SubpassDependency(
			VK_SUBPASS_EXTERNAL,									//Source subpass
			0,														//Destination subpass
			vk::PipelineStageFlagBits::eColorAttachmentOutput,		//Source stage
			vk::PipelineStageFlagBits::eColorAttachmentOutput,		//Destination stage
			{},														//Source access mask
		 	destinationAccessFlags									//Destintation access mask
				
		)
	};

	const vk::RenderPassCreateInfo createInfo(
		{},															//Flags
		attachments.size(), attachments.data(),						//Attachemnts
		subpasses.size(), subpasses.data(),							//Subpasses
		subpassDependencies.size(), subpassDependencies.data()		//Subpass dependencies
	);

	return vulkan.createRenderPass(createInfo);
}
*/
}