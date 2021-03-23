#include <zuazo/Graphics/Sampler.h>

#include <zuazo/Macros.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/Utils/Hasher.h>
#include <zuazo/Graphics/ColorTransfer.h>

#include <bitset>
#include <unordered_map>

namespace Zuazo::Graphics {

Sampler::Sampler(	const Vulkan& vulkan,
					const Image::Plane& plane,
					const InputColorTransfer& colorTransfer,
					ScalingFilter filter )
	: m_filter(getScalingFilter(vulkan, plane, colorTransfer, filter))
	, m_samplerYCbCrConversion(createSamplerYCbCrConversion(vulkan, plane, colorTransfer, m_filter))
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

	//We'll only be able to use a HW filter if there is no transfer function, as
	//otherwise we'll need to linearize before interpolating
	if(colorTransfer.isLinear()) {
		//There is a chance to optimize this
		const auto& formatSupport = vulkan.getFormatSupport().at(plane.getFormat());
		const auto optimalFeatures = formatSupport.optimalTilingFeatures;

		//Decide which flags are required in order to sample with a HW filter
		const bool isYcbCr = requiresYCbCrSamplerConversion(plane.getFormat());
		const vk::FormatFeatureFlags CUBIC_FLAG = 
			isYcbCr ?
			vk::FormatFeatureFlagBits{} :
			vk::FormatFeatureFlagBits::eSampledImageFilterCubicIMG ;
		const vk::FormatFeatureFlags LINEAR_FLAG = 
			isYcbCr ?
			vk::FormatFeatureFlagBits::eSampledImageYcbcrConversionLinearFilter :
			vk::FormatFeatureFlagBits::eSampledImageFilterLinear ;

		switch (filter) {
		case ScalingFilter::CUBIC:
			if(optimalFeatures & CUBIC_FLAG){
				result = vk::Filter::eCubicIMG;
				break;
			} 
			ZUAZO_fallthrough; //else fall back into linear
		
		case ScalingFilter::LINEAR:
			if(optimalFeatures & LINEAR_FLAG) {
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
																	const InputColorTransfer& colorTransfer,
																	vk::Filter filter )
{
	vk::SamplerYcbcrConversion result;
	const auto format = plane.getFormat();

	//Determine if it will be required
	if(requiresYCbCrSamplerConversion(format)) {
		using Index = std::tuple<	vk::Filter,
									vk::Format,
									vk::ComponentSwizzle,
									vk::ComponentSwizzle,
									vk::ComponentSwizzle,
									vk::ComponentSwizzle,
									vk::SamplerYcbcrModelConversion,
									vk::SamplerYcbcrRange >;	
		
		static std::unordered_map<Index, const Utils::StaticId, Utils::Hasher<Index>> ids; 

		const auto swizzle = plane.getSwizzle();
		const auto model = colorTransfer.getYCbCrSamplerModel();
		const auto range = colorTransfer.getYCbCrSamplerRange();

		//Create an index to try to retrieve it from cache
		const Index index (
			filter, format,
			swizzle.r,
			swizzle.g,
			swizzle.b,
			swizzle.a,
			model, range
		);
		const auto& id = ids[index]; //TODO concurrent access

		//Try to obtain it from cache
		result = vulkan.createSamplerYcbcrConversion(id);
		if(!result) {
			//No luck, create it
			constexpr vk::ChromaLocation xChromaLocation = vk::ChromaLocation::eCositedEven;
			constexpr vk::ChromaLocation yChromaLocation = vk::ChromaLocation::eCositedEven;
			constexpr bool forceExplicitReconstruction = false;

			//Ensure that the format is supported
			assert(vulkan.getFormatSupport().at(format).optimalTilingFeatures & vk::FormatFeatureFlagBits::eCositedChromaSamples);
			//FIXME only one of cosited or midpoint samples is *required* to be supported, although both should be

			const vk::SamplerYcbcrConversionCreateInfo createInfo(
				format,						//Image format
				model,						//Model
				range,						//Range
				swizzle,					//Component swizzle
				xChromaLocation,			//X Chroma location
				yChromaLocation,			//Y Chroma location
				filter,						//Chrominance reconstruction filter
				forceExplicitReconstruction	//Force explicit chroma reconstruction
			);

			result = vulkan.createSamplerYcbcrConversion(id, createInfo);

		}
	}

	return result;
}

vk::Sampler Sampler::createSampler(	const Vulkan& vulkan,
									vk::Filter filter,
									vk::SamplerYcbcrConversion samplerYCbCrConversion )
{
	using Index = std::tuple<vk::Filter, vk::SamplerYcbcrConversion>;
	static std::unordered_map<Index, const Utils::StaticId, Utils::Hasher<Index>> ids;

	//Create an index with the relevant parameters and obtain the id
	const Index index(filter, samplerYCbCrConversion);
	const auto& id = ids[index]; //TODO concurrent access

	//Try to retrieve the sampler from cache
	vk::Sampler result = vulkan.createSampler(id);
	if(!result) {
		//Sampler does not exist, create it
		const auto ycbcrConversionInfo = vk::SamplerYcbcrConversionInfo(samplerYCbCrConversion);
		constexpr auto addressMode = vk::SamplerAddressMode::eClampToEdge;

		const auto createInfo = vk::SamplerCreateInfo(
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
			false													//Unnormalized coords
		).setPNext(ycbcrConversionInfo.conversion ? &ycbcrConversionInfo : nullptr);

		result = vulkan.createSampler(id, createInfo);		
	}

	return result;
}

}