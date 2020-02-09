#include <Graphics/RenderPass.h>

namespace Zuazo::Graphics {

RenderPass::RenderPass(	const Vulkan& vulkan,
						const Utils::BufferView<vk::AttachmentDescription>& attachments,
						const Utils::BufferView<vk::SubpassDescription>& subpasses)
	: m_renderPass(createRenderPass(vulkan, attachments, subpasses))
{
}

vk::RenderPass RenderPass::getRenderPass(){
	return *m_renderPass;
}

vk::UniqueRenderPass RenderPass::createRenderPass(	const Vulkan& vulkan,
													const Utils::BufferView<vk::AttachmentDescription>& attachments,
													const Utils::BufferView<vk::SubpassDescription>& subpasses )
{
	const vk::RenderPassCreateInfo createInfo(
		{},
		attachments.size(), attachments.data(),
		subpasses.size(), subpasses.data()
	);

	return vulkan.getDevice().createRenderPassUnique(
		createInfo, nullptr,
		vulkan.getDispatcher()
	);
}


}