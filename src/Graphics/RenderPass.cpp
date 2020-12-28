#include <zuazo/Graphics/RenderPass.h>

#include <zuazo/Utils/StaticId.h>
#include <zuazo/shaders/color_transfer.h>

#include <algorithm>

namespace Zuazo::Graphics {

RenderPass::RenderPass(	const Vulkan& vulkan, 
						Utils::BufferView<const Frame::PlaneDescriptor> planeDescriptors,
						DepthStencilFormat depthStencilFmt,
						vk::ImageLayout finalLayout )
	: m_renderPass(createRenderPass(vulkan, planeDescriptors, depthStencilFmt, finalLayout))
{
}

bool RenderPass::operator==(const RenderPass& other) const noexcept {
	return m_renderPass == other.m_renderPass;
}

bool RenderPass::operator!=(const RenderPass& other) const noexcept {
	return !operator==(other);
}



vk::RenderPass RenderPass::getRenderPass() const {
	return m_renderPass;
}



vk::RenderPass RenderPass::createRenderPass(const Vulkan& vulkan, 
											Utils::BufferView<const Frame::PlaneDescriptor> planeDescriptors,
											DepthStencilFormat depthStencilFmt,
											vk::ImageLayout finalLayout )
{
	//Get an id for this configuration
	static std::unordered_map<uint64_t, const Utils::StaticId> ids;

	uint64_t index = 0;

	constexpr uint64_t COLOR_FORMAT_MULT = 0x100; //4*8 = 32bits
	for(const auto& plane : planeDescriptors) {
		assert(plane.swizzle == vk::ComponentMapping());

		index *= COLOR_FORMAT_MULT;
		index += static_cast<uint64_t>(plane.format) % COLOR_FORMAT_MULT; //TODO this does not work well with extensions, too big numbers.
	}

	constexpr uint64_t DEPTH_STENCIL_MULT = 0x100; //8 bits
	index *= DEPTH_STENCIL_MULT;
	index += static_cast<uint64_t>(depthStencilFmt) % DEPTH_STENCIL_MULT;

	constexpr uint64_t FINAL_LAYOUT_MULT = 0x100; //8bits
	index *= FINAL_LAYOUT_MULT;
	index += static_cast<uint64_t>(finalLayout) % FINAL_LAYOUT_MULT; //TODO this does not work well with extensions, too big numbers.

	const size_t id = ids[index];

	//Check if a renderpass with this id exists
	vk::RenderPass result = vulkan.createRenderPass(id);
	if(!result) {
		//Get information about the image planes
		const auto depthStencilFormat = toVulkan(depthStencilFmt);

		//Get the element count for the arrays
		const size_t colorAttachmentCount = planeDescriptors.size();
		const size_t depthStencilAttachmentCount = (depthStencilFormat == vk::Format::eUndefined) ? 0 : 1;
		const size_t attachmentCount = colorAttachmentCount + depthStencilAttachmentCount;

		//Create the attachments descriptors
		std::vector<vk::AttachmentDescription> attachments;
		attachments.reserve(attachmentCount);

		for(const auto& plane : planeDescriptors) {
			attachments.emplace_back(
				vk::AttachmentDescriptionFlags(),				//Flags
				plane.format,									//Attachemnt format
				vk::SampleCountFlagBits::e1,					//Sample count
				vk::AttachmentLoadOp::eClear,					//Color attachment load operation
				vk::AttachmentStoreOp::eStore,					//Color attachemnt store operation
				vk::AttachmentLoadOp::eDontCare,				//Stencil attachment load operation
				vk::AttachmentStoreOp::eDontCare,				//Stencil attachment store operation
				vk::ImageLayout::eUndefined,					//Initial layout
				finalLayout										//Final layout
			);
		}

		if(depthStencilFormat != vk::Format::eUndefined) {
			attachments.emplace_back(
				vk::AttachmentDescriptionFlags(),				//Flags
				depthStencilFormat,								//Attachemnt format
				vk::SampleCountFlagBits::e1,					//Sample count
				vk::AttachmentLoadOp::eClear,					//Depth attachment load operation
				vk::AttachmentStoreOp::eDontCare,				//Depth attachemnt store operation
				vk::AttachmentLoadOp::eClear,					//Stencil attachment load operation
				vk::AttachmentStoreOp::eDontCare,				//Stencil attachment store operation
				vk::ImageLayout::eUndefined,					//Initial layout
				vk::ImageLayout::eDepthStencilAttachmentOptimal	//Final layout //TODO maybe undefined?
			);
		}
		assert(attachments.size() == attachmentCount);

		//Create the color attachment references for the subpass
		std::array<vk::AttachmentReference, ct_OUTPUT_COUNT> colorAttachmentReferences;
		assert(colorAttachmentCount <= colorAttachmentReferences.size());

		for(size_t i = 0; i < colorAttachmentCount; ++i) {
			colorAttachmentReferences[i] = vk::AttachmentReference(
				i, 												//Attachments index
				vk::ImageLayout::eColorAttachmentOptimal 		//Attachemnt layout
			);
		}
		for(size_t i = colorAttachmentCount; i < colorAttachmentReferences.size(); ++i) {
			colorAttachmentReferences[i] = vk::AttachmentReference(
				VK_ATTACHMENT_UNUSED, 							//Attachments index
				vk::ImageLayout::eColorAttachmentOptimal 		//Attachemnt layout
			);	
		}

		//Create the depth and stencil attachment reference
		const vk::AttachmentReference depthStencilAttachmentReference(
			colorAttachmentCount, 								//Attachments index
			vk::ImageLayout::eDepthStencilAttachmentOptimal 	//Attachemnt layout
		);

		//Create the subpass descriptor
		const std::array subpassDescriptors {
			vk::SubpassDescription(
				{},																			//Flags
				vk::PipelineBindPoint::eGraphics,											//Pipeline bind point
				0, nullptr,																	//Input attachments
				colorAttachmentReferences.size(), colorAttachmentReferences.data(), 		//Color attachments
				nullptr,																	//Resolve attachment
				depthStencilAttachmentCount ? &depthStencilAttachmentReference : nullptr, 	//Depth-stencil attachment
				0, nullptr																	//Preserve attachments
			)
		};

		//Create the renderpass
		const vk::RenderPassCreateInfo createInfo(
			{},													//Flags
			attachments.size(), attachments.data(),				//Attachments
			subpassDescriptors.size(), subpassDescriptors.data(), //Subpasses
			0, nullptr 											//Subpass dependencies
		);

		result = vulkan.createRenderPass(id, createInfo);
	}

	assert(result);
	return result;
}

}