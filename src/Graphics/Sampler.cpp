#include <zuazo/Graphics/Sampler.h>

#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Utils/StaticId.h>

#include <bitset>
#include <unordered_map>

namespace Zuazo::Graphics {

/*
 * Sampler::Impl
 */

struct Sampler::Impl {
	vk::SamplerYcbcrConversion			ycbcrConversion;
	vk::Sampler							sampler;
	StagedBuffer						uniformBuffer;



	vk::SamplerYcbcrConversion getYCbCrSamplerConversion() const noexcept {
		return ycbcrConversion;
	}

	vk::Sampler getSampler() const noexcept {
		return sampler;
	}

private:
	static vk::SamplerYcbcrConversion createYCbCrSamplerConversion(	const Vulkan& vulkan,
																	ScalingFilter filter )
	{
		vk::SamplerYcbcrConversion result;

		if(true) { //Determine if it will be required
			constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
				vk::FormatFeatureFlagBits::eSampledImage |			//Well sample the image from the shader
				vk::FormatFeatureFlagBits::eDisjoint |				//Well bind each plane separately
				vk::FormatFeatureFlagBits::eCositedChromaSamples ;	//We only support cosited chroma samples
			const auto& supportedFormats = vulkan.listSupportedFormatsOptimal(DESIRED_FLAGS);

			//For binary search:
			assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend()));

			const vk::Format format = vk::Format::eUndefined; //TODO

			//Check if the format is supported for ycbcr sampling
			if(std::binary_search(supportedFormats.cbegin(), supportedFormats.cend(), format)) {
				const auto model = toVulkan(ColorModel::RGB); //TODO
				const auto range = toVulkan(ColorRange::FULL); //TODO
				const vk::Filter chromaFilter = vk::Filter::eNearest; //TODO
				const vk::ComponentMapping swizzle = vk::ComponentMapping(); //TODO maybe the same as the image view

				constexpr vk::ChromaLocation xChromaLocation = vk::ChromaLocation::eCositedEven;
				constexpr vk::ChromaLocation yChromaLocation = vk::ChromaLocation::eCositedEven;
				constexpr bool forceExplicitReconstruction = false;

				const vk::SamplerYcbcrConversionCreateInfo createInfo(
					format,						//Image format
					model,						//Model
					range,						//Range
					swizzle,					//Component swizzle
					xChromaLocation,			//X Chroma location
					yChromaLocation,			//Y Chroma location
					chromaFilter,				//Chrominance reconstruction filter
					forceExplicitReconstruction	//Force explicit chroma reconstruction
				);
			}
		}

		return result;
	}


	static vk::Sampler	createSampler(	const Vulkan& vulkan, 
										ScalingFilter filter,
										vk::SamplerYcbcrConversion ycbcrConversion )
	{
		using Index = std::bitset<sizeof(size_t)*Utils::getByteSize()*2>;
		static std::unordered_map<Index, const Utils::StaticId> ids; 

		//Create an index with the relevant parameters and obtain the id
		Index index;
		index |= reinterpret_cast<const uintptr_t&>(ycbcrConversion);
		index <<= sizeof(size_t)*Utils::getByteSize();
		index |= static_cast<size_t>(filter);
		const auto& id = ids[index]; //TODO concurrent access

		//Try to retrieve the sampler from cache
		vk::Sampler result = vulkan.createSampler(id);
		if(!result) {
			//Sampler does not exist, create it
			const vk::Filter filter = vk::Filter::eNearest; //TODO

			const vk::SamplerYcbcrConversionInfo ycbcrConversionInfo(ycbcrConversion);
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
	}


};


/*
 * Sampler
 */





vk::SamplerYcbcrConversion Sampler::getYCbCrSamplerConversion() const noexcept {
	return m_impl->getYCbCrSamplerConversion();
}

vk::Sampler Sampler::getSampler() const noexcept {
	return m_impl->getSampler();
}



















}