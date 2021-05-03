#pragma once

#include "Vulkan.h"
#include "Frame.h"
#include "RenderPass.h"
#include "CommandBuffer.h"
#include "../Utils/Pimpl.h"

#include <vector>

namespace Zuazo::Graphics {

class TargetFrame 
	: public Frame 
{
public:
	class Cache;

	TargetFrame(const Vulkan& vulkan,
				std::shared_ptr<const Descriptor> desc,
				std::shared_ptr<const Cache> cache = nullptr,
				std::shared_ptr<void> usrPtr = nullptr  );
	TargetFrame(const TargetFrame& other) = delete;
	TargetFrame(TargetFrame&& other) noexcept;
	virtual ~TargetFrame(); 

	TargetFrame& 								operator=(const TargetFrame& other) = delete;
	TargetFrame&								operator=(TargetFrame&& other) noexcept;

	bool										waitCompletion(uint64_t timeo) const;

	void										beginRenderPass(	vk::CommandBuffer cmd, 
																	vk::Rect2D renderArea,
																	Utils::BufferView<const vk::ClearValue> clearValues,
																	vk::SubpassContents contents ) const noexcept;
	void										endRenderPass(vk::CommandBuffer cmd) const noexcept;
	void										draw(std::shared_ptr<const CommandBuffer> cmd);

	static std::shared_ptr<const Cache>			createCache(const Vulkan& vulkan, 
															const Frame::Descriptor& frameDesc,
															DepthStencilFormat depthStencilFmt  );
	static const RenderPass&					getRenderPass(const Cache& cache) noexcept;

	static Utils::Discrete<ColorFormat> 		getSupportedFormats(const Vulkan& vulkan);
	static Utils::Discrete<ColorFormat> 		getSupportedSrgbFormats(const Vulkan& vulkan);

private:
	struct Impl;
	Utils::Pimpl<Impl>							m_impl;

};

}