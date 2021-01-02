#pragma once

#include "Vulkan.h"
#include "Image.h"
#include "DepthStencil.h"
#include "RenderPass.h"

namespace Zuazo::Graphics {

class Framebuffer {
public:
	Framebuffer() = default;
	Framebuffer(const Vulkan& vulkan,
				Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
				const Image& image,
				const DepthStencil* depthStencil,
				RenderPass renderPass );
	Framebuffer(const Framebuffer& other) = delete;
	Framebuffer(Framebuffer&& other) = default;
	~Framebuffer() = default;

	Framebuffer&					operator=(const Framebuffer& other) = delete;
	Framebuffer&					operator=(Framebuffer&& other) = default;

	vk::Framebuffer					get() const noexcept;

private:
	vk::UniqueFramebuffer			m_framebuffer;

	static vk::UniqueFramebuffer	createFramebuffer(	const Vulkan& vulkan,
														Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
														const Image& image,
														const DepthStencil* depthStencil,
														RenderPass renderPass );

};

}