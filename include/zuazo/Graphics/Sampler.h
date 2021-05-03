#pragma once

#include "Vulkan.h"
#include "Image.h"
#include "../ColorRange.h"
#include "../ColorModel.h"
#include "../ColorTransferFunction.h"
#include "../ScalingFilter.h"

namespace Zuazo::Graphics {

class InputColorTransfer;

class Sampler {
public:
	Sampler() = default;
	Sampler(const Vulkan& vulkan,
			const Image::Plane& plane,
			ColorRange range,
			ColorModel model,
			ColorTransferFunction colorTransferFunction,
			ScalingFilter filter );
	Sampler(const Sampler& other) = default;
	~Sampler() = default;

	Sampler&							operator=(const Sampler& other) = default;

	vk::SamplerYcbcrModelConversion		getModel() const noexcept;
	vk::SamplerYcbcrRange				getRange() const noexcept;
	vk::Filter							getFilter() const noexcept;
	vk::SamplerYcbcrConversion			getSamplerYCbCrConversion() const noexcept;
	vk::Sampler							getSampler() const noexcept;

	static vk::Sampler					getNearestSampler(const Vulkan& vulkan);

private:
	vk::SamplerYcbcrModelConversion		m_model;
	vk::SamplerYcbcrRange				m_range;
	vk::Filter							m_filter;
	vk::SamplerYcbcrConversion			m_samplerYCbCrConversion;
	vk::Sampler							m_sampler;


	static bool							usesYCbCrSampler(vk::Format format, vk::SamplerYcbcrModelConversion model) noexcept;
	static vk::SamplerYcbcrModelConversion	getModel(ColorModel model) noexcept;
	static vk::SamplerYcbcrRange		getRange(	ColorRange range, 
													vk::SamplerYcbcrModelConversion& model ) noexcept;
	static vk::Filter					getFilter(	const Vulkan& vulkan,
													const Image::Plane& plane,
													ColorTransferFunction colorTransferFunction,
													ScalingFilter filter,
													vk::SamplerYcbcrModelConversion model );

	static vk::SamplerYcbcrConversion	createSamplerYCbCrConversion(	const Vulkan& vulkan,
																		const Image::Plane& plane,
																		vk::SamplerYcbcrModelConversion model,
																		vk::SamplerYcbcrRange range,
																		vk::Filter filter );
	static vk::Sampler					createSampler(	const Vulkan& vulkan,
														vk::Filter filter,
														vk::SamplerYcbcrConversion samplerYCbCrConversion );

};

}