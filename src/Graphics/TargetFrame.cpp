#include <zuazo/Graphics/TargetFrame.h>

#include <algorithm>

namespace Zuazo::Graphics {

/*
 * TargetFrame
 */

TargetFrame::TargetFrame(	const Vulkan& vulkan,
							std::shared_ptr<const Descriptor> desc,
							std::shared_ptr<const Buffer> colorTransfer,
							Utils::BufferView<const Frame::PlaneDescriptor> planes,
							std::shared_ptr<const vk::UniqueRenderPass> renderPass )
	: Frame(
		vulkan,
		std::move(desc),
		std::move(colorTransfer),
		planes,
		vk::ImageUsageFlagBits::eColorAttachment )
	, m_renderPass(std::move(renderPass))
	, m_frameBuffer(createFrameBuffer(vulkan, planes, m_renderPass, getImageViews()))
	, m_renderComplete(vulkan.createFence(true))
{
}

TargetFrame::~TargetFrame() {
	waitDependecies();
}




vk::UniqueFramebuffer TargetFrame::createFrameBuffer(	const Graphics::Vulkan& vulkan,
														Utils::BufferView<const PlaneDescriptor> planes,
														const std::shared_ptr<const vk::UniqueRenderPass>& renderPass,
														const std::vector<vk::UniqueImageView>& imageViews )
{
	assert(renderPass);
	assert(*renderPass);
	assert(imageViews.size() == planes.size());
	const auto attachmentCount = imageViews.size();

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
TargetFrame::RenderPass::RenderPass(const Vulkan& vulkan,
									Utils::BufferView<const PlaneDescriptor> planes, 
									vk::Format depthStencilFmt )
	: m_renderPass(createRenderPass(vulkan, planes, depthStencilFmt))
	, m_depthStencilImage(createDepthStencilImage(vulkan, planes, depthStencilFmt))
	, m_depthStencilImageMemory(createDepthStencilMemory(vulkan, *m_depthStencilImage))
	, m_depthStencilImageView(createDepthStencilImageView(vulkan, *m_depthStencilImage, depthStencilFmt))
{
}



vk::RenderPass TargetFrame::RenderPass::getRenderPass() const {
	return *m_renderPass;
}

vk::Image TargetFrame::RenderPass::getDepthStencilImage() const {
	return *m_depthStencilImage;
}

vk::ImageView TargetFrame::RenderPass::getDepthStencilImageView() const {
	return *m_depthStencilImageView;
}



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

vk::UniqueImage TargetFrame::RenderPass::createDepthStencilImage(	const Vulkan& vulkan,
																	Utils::BufferView<const PlaneDescriptor> planes, 
																	vk::Format depthStencilFmt )
{
	//Ensure a valid format is provided
	if(depthStencilFmt == vk::Format::eUndefined) return vk::UniqueImage();

	constexpr vk::ImageUsageFlags usageFlags = 
		vk::ImageUsageFlagBits::eDepthStencilAttachment;

	const vk::ImageCreateInfo createInfo(
		{},											//Flags
		vk::ImageType::e2D,							//Image type
		depthStencilFmt,							//Pixel format
		vk::Extent3D(planes[0].extent, 1), 			//Extent
		1,											//Mip levels
		1,											//Array layers
		vk::SampleCountFlagBits::e1,				//Sample count
		vk::ImageTiling::eOptimal,					//Tiling
		usageFlags,									//Usage
		vk::SharingMode::eExclusive,				//Sharing mode
		0, nullptr,									//Queue family indices
		vk::ImageLayout::eUndefined					//Initial layout
	);

	return vulkan.createImage(createInfo);
}

vk::UniqueDeviceMemory TargetFrame::RenderPass::createDepthStencilMemory(	const Vulkan& vulkan,
																			vk::Image image )
{
	//Ensure that the provided image is valid
	if(!image) return vk::UniqueDeviceMemory();

	constexpr vk::MemoryPropertyFlags memoryProperties =
			vk::MemoryPropertyFlagBits::eDeviceLocal;

	const auto requirements = vulkan.getMemoryRequirements(image);
	auto result = vulkan.allocateMemory(requirements, memoryProperties);
	vulkan.bindMemory(image, *result);

	return result;
}

vk::UniqueImageView TargetFrame::RenderPass::createDepthStencilImageView(const Vulkan& vulkan,
																		vk::Image image,
																		vk::Format depthStencilFmt ) 
{
	//Ensure that the provided image is valid
	if(!image) return vk::UniqueImageView();

	const vk::ImageAspectFlags aspectMask = 
		(hasDepth(depthStencilFmt) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits{}) |
		(hasStencil(depthStencilFmt) ? vk::ImageAspectFlagBits::eStencil : vk::ImageAspectFlagBits{});

	assert(aspectMask != vk::ImageAspectFlags());

	const vk::ImageViewCreateInfo createInfo(
		{},												//Flags
		image,											//Image
		vk::ImageViewType::e2D,							//ImageView type
		depthStencilFmt,								//Image format
		vk::ComponentSwizzle(),							//Swizzle
		vk::ImageSubresourceRange(						//Image subresources
			aspectMask,										//Aspect mask
			0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
		)
	);

	return vulkan.createImageView(createInfo);
}
*/
}