#include <Graphics/RenderPass.h>

namespace Zuazo::Graphics {

RenderPass::RenderPass(	const Vulkan& vulkan,
						const Utils::BufferView<vk::AttachmentDescription>& attachments,
						const Utils::BufferView<vk::SubpassDescription>& subpasses,
						const Utils::BufferView<vk::SubpassDependency>& subpassDependencies )
	: m_renderPass(createRenderPass(vulkan, attachments, subpasses, subpassDependencies))
{
}

vk::RenderPass RenderPass::getRenderPass(){
	return *m_renderPass;
}

vk::UniqueRenderPass RenderPass::createRenderPass(	const Vulkan& vulkan,
													const Utils::BufferView<vk::AttachmentDescription>& attachments,
													const Utils::BufferView<vk::SubpassDescription>& subpasses,
													const Utils::BufferView<vk::SubpassDependency>& subpassDependencies )
{
	const vk::RenderPassCreateInfo createInfo(
		{},
		attachments.size(), attachments.data(),
		subpasses.size(), subpasses.data(),
		subpassDependencies.size(), subpassDependencies.data()
	);

	return vulkan.getDevice().createRenderPassUnique(
		createInfo, nullptr,
		vulkan.getDispatcher()
	);
}


}