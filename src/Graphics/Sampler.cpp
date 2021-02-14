#include <zuazo/Graphics/Sampler.h>

#include <zuazo/Macros.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/Graphics/ColorTransfer.h>

#include <bitset>
#include <unordered_map>

namespace Zuazo::Graphics {

Sampler::Sampler(	const Vulkan& vulkan,
					const Image::Plane& plane,
					const InputColorTransfer& colorTransfer,
					ScalingFilter filter )
	: m_filter(getScalingFilter(vulkan, plane, colorTransfer, filter))
	, m_samplerYCbCrConversion(createSamplerYCbCrConversion(vulkan, plane, colorTransfer))
	, m_sampler(createSampler(vulkan, m_filter, m_samplerYCbCrConversion))
{
	if(m_samplerYCbCrConversion) {
		assert(m_filter == vk::Filter::eNearest);
	}
}


vk::Filter Sampler::getFilter() const noexcept {
	return m_filter;
}

vk::SamplerYcbcrConversion Sampler::getSamplerYCbCrConversion() const noexcept {
	return m_samplerYCbCrConversion;
}

vk::Sampler Sampler::getSampler() const noexcept {
	return m_sampler;
}


vk::Sampler Sampler::getNearestSampler(const Vulkan& vulkan) {
	//This is a sampler guranteed to be supported
	return createSampler(vulkan, vk::Filter::eNearest, nullptr);
}


vk::Filter Sampler::getScalingFilter(	const Vulkan& vulkan,
										const Image::Plane& plane,
										const InputColorTransfer& colorTransfer,
										ScalingFilter filter )
{
	vk::Filter result = vk::Filter::eNearest;

	if(colorTransfer.isPassthough()) {
		//There is a chance to optimize this
		const auto& formatSupport = vulkan.getFormatSupport().at(plane.getFormat());
		const auto optimalFeatures = formatSupport.optimalTilingFeatures;

		constexpr vk::FormatFeatureFlags CUBIC_FLAGS = vk::FormatFeatureFlagBits::eSampledImageFilterCubicIMG;
		constexpr vk::FormatFeatureFlags LINEAR_FLAGS = vk::FormatFeatureFlagBits::eSampledImageFilterLinear;

		switch (filter) {
		case ScalingFilter::CUBIC:
			if(optimalFeatures | CUBIC_FLAGS){
				result = vk::Filter::eCubicIMG;
				break;
			} 
			ZUAZO_fallthrough; //else fall back into linear
		
		case ScalingFilter::LINEAR:
			if(optimalFeatures | LINEAR_FLAGS) {
				result = vk::Filter::eLinear;
				break;
			}
			ZUAZO_fallthrough; //else fall back into nearest

		default:
			result = vk::Filter::eNearest;
			break;

		}
	}

	return result;
}

vk::SamplerYcbcrConversion Sampler::createSamplerYCbCrConversion(	const Vulkan& vulkan,
																	const Image::Plane& plane,
																	const InputColorTransfer& colorTransfer )
{
	vk::SamplerYcbcrConversion result;

	const vk::Format format = plane.getFormat();
	const vk::ComponentMapping swizzle = plane.getSwizzle();
	const auto model = toVulkan(ColorModel::RGB); //TODO
	const auto range = toVulkan(ColorRange::FULL); //TODO

	//Determine if it will be required
	if(	model > vk::SamplerYcbcrModelConversion::eRgbIdentity || 
		range > vk::SamplerYcbcrRange::eItuFull || 
		requiresYCbCrSamplerConversion(format) ) 
	{
		constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
			vk::FormatFeatureFlagBits::eSampledImage |			//Well sample the image from the shader
			vk::FormatFeatureFlagBits::eCositedChromaSamples ;	//We only support cosited chroma samples

		const auto formatFeatures = vulkan.getFormatSupport().at(format).optimalTilingFeatures;

		//Check if the format is supported for ycbcr sampling
		if((formatFeatures & DESIRED_FLAGS) == DESIRED_FLAGS) {
			using Index = std::bitset<sizeof(uint32_t)*Utils::getByteSize()*8>;
			static std::unordered_map<Index, const Utils::StaticId> ids; 

			//Create an index to try to retrieve it from cache
			Index index;
			index |= static_cast<uint32_t>(format);
			std::for_each(
				reinterpret_cast<const vk::ComponentSwizzle*>(&swizzle),
				reinterpret_cast<const vk::ComponentSwizzle*>(&swizzle + 1),
				[&index] (vk::ComponentSwizzle componentSwizzle) {
					index <<= sizeof(uint32_t)*Utils::getByteSize();
					index |= static_cast<uint32_t>(componentSwizzle);
				}
			);
			index <<= sizeof(uint32_t)*Utils::getByteSize();
			index |= static_cast<uint32_t>(model);
			index <<= sizeof(uint32_t)*Utils::getByteSize();
			index |= static_cast<uint32_t>(range);
			const auto& id = ids[index]; //TODO concurrent access

			//Try to obtain it from cache
			result = vulkan.createSamplerYcbcrConversion(id);
			if(!result) {
				//No luck, create it
				constexpr vk::ChromaLocation xChromaLocation = vk::ChromaLocation::eCositedEven;
				constexpr vk::ChromaLocation yChromaLocation = vk::ChromaLocation::eCositedEven;
				constexpr vk::Filter chromaReconstructionFilter = vk::Filter::eNearest; //We'll always interpolate manually
				constexpr bool forceExplicitReconstruction = false;

				const vk::SamplerYcbcrConversionCreateInfo createInfo(
					format,						//Image format
					model,						//Model
					range,						//Range
					swizzle,					//Component swizzle
					xChromaLocation,			//X Chroma location
					yChromaLocation,			//Y Chroma location
					chromaReconstructionFilter,	//Chrominance reconstruction filter
					forceExplicitReconstruction	//Force explicit chroma reconstruction
				);

				result = vulkan.createSamplerYcbcrConversion(id, createInfo);
			}
		}
	}

	return result;
}

vk::Sampler Sampler::createSampler(	const Vulkan& vulkan,
									vk::Filter filter,
									vk::SamplerYcbcrConversion samplerYCbCrConversion )
{
	using Index = std::bitset<sizeof(size_t)*Utils::getByteSize()*2>;
	static std::unordered_map<Index, const Utils::StaticId> ids; 

	//Create an index with the relevant parameters and obtain the id
	Index index;
	index |= static_cast<size_t>(filter);
	index <<= sizeof(size_t)*Utils::getByteSize();
	index |= reinterpret_cast<const uintptr_t&>(samplerYCbCrConversion);
	const auto& id = ids[index]; //TODO concurrent access

	//Try to retrieve the sampler from cache
	vk::Sampler result = vulkan.createSampler(id);
	if(!result) {
		//Sampler does not exist, create it
		const vk::SamplerYcbcrConversionInfo ycbcrConversionInfo(samplerYCbCrConversion);
		constexpr auto addressMode = vk::SamplerAddressMode::eClampToEdge;

		vk::SamplerCreateInfo createInfo(
			{},														//Flags
			filter, filter,											//Min/Mag filter
			vk::SamplerMipmapMode::eNearest,						//Mipmap mode
			addressMode,											//U address mode
			addressMode,											//V address mode
			addressMode,											//W address mode
			0.0f,													//Mip LOD bias
			false,													//Enable anisotropy
			0.0f,													//Max anisotropy
			false,													//Compare enable
			vk::CompareOp::eNever,									//Compare operation
			0.0f, 0.0f,												//Min/Max LOD
			vk::BorderColor::eFloatTransparentBlack,				//Boreder color
			false													//Unormalized coords
		);

		if(ycbcrConversionInfo.conversion) {
			createInfo.setPNext(&ycbcrConversionInfo);
		}

		result = vulkan.createSampler(id, createInfo);		
	}

	return result;
}

}