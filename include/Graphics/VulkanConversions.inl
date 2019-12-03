#include "VulkanConversions.h"

#include <utility>

namespace Zuazo::Graphics {

constexpr vk::Extent2D toVulkan(const Resolution& res){
	return vk::Extent2D(
		res.width,
		res.height
	);
}

constexpr Resolution fromVulkan(const vk::Extent2D& res){
	return Resolution(
		res.width,
		res.height
	);
}

constexpr std::pair<PixelFormat, VulkanPixelFmtRep> pixelFormatConvertTable[] = {
	// 		Zuazo pixel format			| 		Vulkan pixel format				| 	Swizzle mask
	{ PixelFormats::R8G8B8, 			{ vk::Format::eR8G8B8Unorm,				ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::B8G8R8, 			{ vk::Format::eB8G8R8Unorm,				ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::R8G8B8A8, 			{ vk::Format::eR8G8B8A8Unorm,			ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::B8G8R8A8, 			{ vk::Format::eB8G8R8A8Unorm,			ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::A8R8G8B8, 			{ vk::Format::eR8G8B8A8Unorm,			ZUAZO_VK_SWIZZLE(A, R, G, B) } },
	{ PixelFormats::A8B8G8R8, 			{ vk::Format::eR8G8B8A8Unorm,			ZUAZO_VK_SWIZZLE(A, B, G, R) } },
	{ PixelFormats::R16G16B16, 			{ vk::Format::eR16G16B16Unorm,			ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::R16G16B16A16, 		{ vk::Format::eR16G16B16A16Unorm,		ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::B16G16R16A16, 		{ vk::Format::eR16G16B16A16Unorm,		ZUAZO_VK_SWIZZLE(B, G, R, A) } },
	{ PixelFormats::A16R16G16B16, 		{ vk::Format::eR16G16B16A16Unorm,		ZUAZO_VK_SWIZZLE(A, R, G, B) } },
	{ PixelFormats::A16B16G16R16, 		{ vk::Format::eR16G16B16A16Unorm,		ZUAZO_VK_SWIZZLE(A, B, G, R) } },

	{ PixelFormats::R16fG16fB16f, 		{ vk::Format::eR16G16B16Sfloat,			ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::R16fG16fB16fA16f,	{ vk::Format::eR16G16B16A16Sfloat,		ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::R32fG32fB32f, 		{ vk::Format::eR32G32B32Sfloat,			ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::R32fG32fB32fA32f, 	{ vk::Format::eR32G32B32A32Sfloat,		ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::R64fG64fB64f, 		{ vk::Format::eR64G64B64Sfloat,			ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::R64fG64fB64fA64f, 	{ vk::Format::eR64G64B64A64Sfloat,		ZUAZO_VK_SWIZZLE_IDENTITY } },

	{ PixelFormats::R4G4B4A4_16,		{ vk::Format::eR4G4B4A4UnormPack16,		ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::B4G4R4A4_16,		{ vk::Format::eB4G4R4A4UnormPack16,		ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::R5G6B5_16,			{ vk::Format::eR5G6B5UnormPack16,		ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::B5G6R5_16,			{ vk::Format::eB5G6R5UnormPack16,		ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::A8B8G8R8_32,		{ vk::Format::eA8B8G8R8UnormPack32,		ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::A2R10G10B10_32,		{ vk::Format::eA2R10G10B10UnormPack32,	ZUAZO_VK_SWIZZLE_IDENTITY } },
	{ PixelFormats::A2B10G10R10_32,		{ vk::Format::eA2B10G10R10UnormPack32,	ZUAZO_VK_SWIZZLE_IDENTITY } },

	//TODO add swizzled formats
	//TODO add planar formats
	//TODO add YCbCr formats

	//Add here

	{ PixelFormats::NONE, 				{ vk::Format::eUndefined, 				{} } }
};


constexpr VulkanPixelFmtRep toVulkan(const PixelFormat& fmt){
	size_t i = 0;

	for(; pixelFormatConvertTable[i].first; i++){
		if(areCompatible(pixelFormatConvertTable[i].first, fmt)){
			break;
		}
	}

	return pixelFormatConvertTable[i].second;
}

constexpr PixelFormat fromVulkan(const VulkanPixelFmtRep& fmt){
	size_t i = 0;

	for(; pixelFormatConvertTable[i].first; i++){
		if(pixelFormatConvertTable[i].second == fmt){
			break;
		}
	}

	return pixelFormatConvertTable[i].first;
}

}