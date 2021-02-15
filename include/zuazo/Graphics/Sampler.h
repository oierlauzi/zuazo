#pragma once

#include "Vulkan.h"
#include "Image.h"
#include "../ScalingFilter.h"

namespace Zuazo::Graphics {

class InputColorTransfer;

class Sampler {
public:
	Sampler() = default;
	Sampler(const Vulkan& vulkan,
			const Image::Plane& plane,
			const InputColorTransfer& colorTransfer,
			ScalingFilter filter );
	Sampler(const Sampler& other) = default;
	~Sampler() = default;

	Sampler&							operator=(const Sampler& other) = default;

	vk::Filter							getFilter() const noexcept;
	vk::SamplerYcbcrConversion			getSamplerYCbCrConversion() const noexcept;
	vk::Sampler							getSampler() const noexcept;

	static vk::Sampler					getNearestSampler(const Vulkan& vulkan);

private:
	vk::Filter							m_filter;
	vk::SamplerYcbcrConversion			m_samplerYCbCrConversion;
	vk::Sampler							m_sampler;

	static vk::Filter					getScalingFilter(	const Vulkan& vulkan,
															const Image::Plane& plane,
															const InputColorTransfer& colorTransfer,
															ScalingFilter filter );

	static vk::SamplerYcbcrConversion	createSamplerYCbCrConversion(	const Vulkan& vulkan,
																		const Image::Plane& plane,
																		const InputColorTransfer& colorTransfer,
																		vk::Filter filter );
	static vk::Sampler					createSampler(	const Vulkan& vulkan,
														vk::Filter filter,
														vk::SamplerYcbcrConversion samplerYCbCrConversion );

};

}