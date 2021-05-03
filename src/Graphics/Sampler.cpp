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
					ColorRange range,
					ColorModel model,
					ColorTransferFunction colorTransferFunction,
					ScalingFilter filter )
	: m_model(getModel(model))
	, m_range(getRange(range, m_model))
	, m_filter(getFilter(vulkan, plane, colorTransferFunction, filter, m_model))
	, m_samplerYCbCrConversion(createSamplerYCbCrConversion(vulkan, plane, m_model, m_range, m_filter))
	, m_sampler(createSampler(vulkan, m_filter, m_samplerYCbCrConversion))
{
	if(!m_samplerYCbCrConversion) {
		//We are not using the YCbCr sampler, so no range expansion nor 
		//model conversion will be performed
		m_range = vk::SamplerYcbcrRange::eItuFull;
		m_model = vk::SamplerYcbcrModelConversion::eRgbIdentity;
	}
}


vk::SamplerYcbcrModelConversion Sampler::getModel() const noexcept {
	return m_model;
}

vk::SamplerYcbcrRange Sampler::getRange() const noexcept {
	return m_range;
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


bool Sampler::usesYCbCrSampler(vk::Format format, vk::SamplerYcbcrModelConversion model) noexcept {
	//YCbCr sampler will be used under 2 conditions: If the model is not RGB identity, in which case 
	//at least it will be used to substract 0.5 to Cb and Cr (it may also perform range expansion and/or
	//model conversion). The other case is when the format requires to use such a sampler, normally
	//because it is a planar or interleaved format.
	return 	model != vk::SamplerYcbcrModelConversion::eRgbIdentity ||
			requiresYCbCrSamplerConversion(format) ;
}

vk::SamplerYcbcrModelConversion	Sampler::getModel(ColorModel model) noexcept {
	const auto identity = 	isYCbCr(model) ? 
							vk::SamplerYcbcrModelConversion::eYcbcrIdentity : 
							vk::SamplerYcbcrModelConversion::eRgbIdentity ;
	return Math::max(identity, toVulkan(model)); //Do not return unsupported values
}

vk::SamplerYcbcrRange Sampler::getRange(ColorRange range, 
										vk::SamplerYcbcrModelConversion& model ) noexcept
{
	vk::SamplerYcbcrRange result = vk::SamplerYcbcrRange::eItuFull;

	//In order to use the narrow range, the color model must not be RGB,
	//as it has the same headroom for all components, whilst YCbCr does 
	//not. Vulkan will always use the YCbCr expansion, with alpha passthough
	if(model != vk::SamplerYcbcrModelConversion::eRgbIdentity) {
		result = toVulkan(range);

		//If the conversion failed, reset the range and model. The 
		//whole conversion will have to be performed manually.
		if(result < vk::SamplerYcbcrRange::eItuFull) {
			model = vk::SamplerYcbcrModelConversion::eRgbIdentity;
			result = vk::SamplerYcbcrRange::eItuFull;
		}
	}

	return result;
}

vk::Filter Sampler::getFilter(	const Vulkan& vulkan,
								const Image::Plane& plane,
								ColorTransferFunction colorTransferFunction,
								ScalingFilter filter,
								vk::SamplerYcbcrModelConversion model )
{
	//Nearest sampling is ensured to work in all cases. Avoid checking it
	vk::Filter result = vk::Filter::eNearest;
	if(filter != ScalingFilter::NEAREST) {
		//Check if we're using a sRGB format
		if(fromSrgb(plane.getFormat()) != plane.getFormat()) {
			//Using HW accelerated sRGB linearization
			//From the shader's perspective, this is a linear format
			colorTransferFunction = ColorTransferFunction::LINEAR;
		}

		//We'll only be able to use a HW filter if there is no transfer function (or
		//its linearization is supported by the TMU), as otherwise we'll need to 
		//linearize before interpolating
		if(colorTransferFunction == ColorTransferFunction::LINEAR) {
			const auto format = plane.getFormat();

			//There is a chance to optimize this
			const auto& formatSupport = vulkan.getFormatSupport().at(format);
			const auto optimalFeatures = formatSupport.optimalTilingFeatures;

			//Decide which flags are required in order to sample with a HW filter
			const bool isYCbCr = usesYCbCrSampler(format, model);
			const vk::FormatFeatureFlags CUBIC_FLAG = 
				isYCbCr ?
				vk::FormatFeatureFlagBits{} :
				vk::FormatFeatureFlagBits::eSampledImageFilterCubicEXT ; //Rarely supported
			const vk::FormatFeatureFlags LINEAR_FLAG = 
				isYCbCr ?
				vk::FormatFeatureFlagBits::eSampledImageYcbcrConversionLinearFilter :
				vk::FormatFeatureFlagBits::eSampledImageFilterLinear ;

			switch (filter) {
			case ScalingFilter::CUBIC:
				if(optimalFeatures & CUBIC_FLAG){
					result = vk::Filter::eCubicEXT;
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
	}

	return result;
}



vk::SamplerYcbcrConversion Sampler::createSamplerYCbCrConversion(	const Vulkan& vulkan,
																	const Image::Plane& plane,
																	vk::SamplerYcbcrModelConversion model,
																	vk::SamplerYcbcrRange range,
																	vk::Filter filter )
{
	vk::SamplerYcbcrConversion result;
	const auto format = plane.getFormat();
	const auto& swizzle = plane.getSwizzle();
	const auto& formatSupport = vulkan.getFormatSupport().at(format);

	//Determine if the YCbCr sampler will be required.
	bool create;
	if(usesYCbCrSampler(format, model))	{
		//A YCbCr sampler may become handy to perform color model conversions. Check for support.
		//Any of those two flags will signal that it is supported
		constexpr auto desiredFlags = 	vk::FormatFeatureFlagBits::eCositedChromaSamples |
										vk::FormatFeatureFlagBits::eMidpointChromaSamples ;
		create = static_cast<bool>(formatSupport.optimalTilingFeatures & desiredFlags);
	} else {
		//No need to create it
		create = false;
	}

	//Create if necessary
	if(create) {
		using Index = std::tuple<	vk::Format,
									vk::ComponentSwizzle,
									vk::ComponentSwizzle,
									vk::ComponentSwizzle,
									vk::ComponentSwizzle,
									vk::SamplerYcbcrModelConversion,
									vk::SamplerYcbcrRange,
									vk::Filter >;	
		
		static std::unordered_map<Index, const Utils::StaticId, Utils::Hasher<Index>> ids; 

		//Create an index to try to retrieve it from cache
		const Index index (
			format,
			swizzle.r,
			swizzle.g,
			swizzle.b,
			swizzle.a,
			model,
			range,
			filter
		);
		const auto& id = ids[index]; //TODO concurrent access

		//Try to obtain it from cache
		result = vulkan.createSamplerYcbcrConversion(id);
		if(!result) {
			//No luck, create it
			//Decide where to sample. It is ensured that one of both is supported. See above
			//FIXME this is not colorimetrically correct. It must be sampled where specified.
			vk::ChromaLocation chromaLocation;
			if(formatSupport.optimalTilingFeatures & vk::FormatFeatureFlagBits::eCositedChromaSamples) {
				chromaLocation = vk::ChromaLocation::eCositedEven;
			} else {
				assert(formatSupport.optimalTilingFeatures & vk::FormatFeatureFlagBits::eMidpointChromaSamples);
				chromaLocation = vk::ChromaLocation::eMidpoint;
			}
			constexpr bool forceExplicitReconstruction = false;			

			const vk::SamplerYcbcrConversionCreateInfo createInfo(
				format,						//Image format
				model,						//Model
				range,						//Range
				swizzle,					//Component swizzle
				chromaLocation,				//X Chroma location
				chromaLocation,				//Y Chroma location
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
			false													//Unormalized coords
		).setPNext(ycbcrConversionInfo.conversion ? &ycbcrConversionInfo : nullptr);

		result = vulkan.createSampler(id, createInfo);		
	}

	return result;
}

}