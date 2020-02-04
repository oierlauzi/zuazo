#include "VulkanConversions.h"

#include "VulkanUtils.h"
#include "../Macros.h"

#include <utility>

namespace Zuazo::Graphics {

constexpr uint32_t toVulkan(Version version){
	return VK_MAKE_VERSION(
		version.getMajor(),
		version.getMinor(),
		version.getPatch()
	);
}

constexpr vk::DebugUtilsMessageSeverityFlagsEXT toVulkan(Verbosity verbosity){
	vk::DebugUtilsMessageSeverityFlagsEXT flags = {};

	if(verbosity >= Verbosity::VERBOSE) 
		flags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
	if(verbosity >= Verbosity::INFO) 
		flags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
	if(verbosity >= Verbosity::WARNING) 
		flags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
	if(verbosity >= Verbosity::ERROR) 
		flags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

	return flags;
}

constexpr Verbosity fromVulkan(vk::DebugUtilsMessageSeverityFlagsEXT verbosity){
	if(verbosity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) 	return Verbosity::VERBOSE;
	if(verbosity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) 	return Verbosity::INFO;
	if(verbosity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) 	return Verbosity::WARNING;
	if(verbosity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) 	return Verbosity::ERROR;
	return Verbosity::SILENT;
}

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

constexpr std::tuple<vk::Format, vk::ComponentMapping> toVulkan(PixelFormat fmt, ColorEncoding enc){
	constexpr vk::ComponentMapping IDENTITY;

	constexpr auto R2Y 			= ZUAZO_CONSTRUCT_SWIZZLE( Identity, R, R, One );

	constexpr auto RG2YX 		= ZUAZO_CONSTRUCT_SWIZZLE( Identity, R, R, One );
	constexpr auto RG2XY 		= ZUAZO_CONSTRUCT_SWIZZLE( G, Identity, G, One );
	constexpr auto RG2YA 		= ZUAZO_CONSTRUCT_SWIZZLE( Identity, R, R, G );
	constexpr auto RG2AY 		= ZUAZO_CONSTRUCT_SWIZZLE( G, Identity, G, R );

	constexpr auto ARGB2XRGB 	= ZUAZO_CONSTRUCT_SWIZZLE( Identity, Identity, Identity, One );
	constexpr auto ARGB2XBGR 	= ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, R, One );
	constexpr auto ARGB2XGRB 	= ZUAZO_CONSTRUCT_SWIZZLE( G, R, Identity, One );
	constexpr auto ARGB2XGBR 	= ZUAZO_CONSTRUCT_SWIZZLE( B, R, G, One );
	constexpr auto ARGB2RGBX 	= ZUAZO_CONSTRUCT_SWIZZLE( A, R, G, One );
	constexpr auto ARGB2BGRX 	= ZUAZO_CONSTRUCT_SWIZZLE( G, R, A, One );
	constexpr auto ARGB2GRBX 	= ZUAZO_CONSTRUCT_SWIZZLE( Identity, A, G, One );
	constexpr auto ARGB2GBRX 	= ZUAZO_CONSTRUCT_SWIZZLE( G, A, R, One );
	constexpr auto ARGB2ARGB 	= IDENTITY;
	constexpr auto ARGB2ABGR 	= ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, R, Identity );
	constexpr auto ARGB2AGRB 	= ZUAZO_CONSTRUCT_SWIZZLE( G, R, Identity, Identity );
	constexpr auto ARGB2AGBR 	= ZUAZO_CONSTRUCT_SWIZZLE( B, R, G, Identity );
	constexpr auto ARGB2RGBA 	= ZUAZO_CONSTRUCT_SWIZZLE( A, R, G, B );
	constexpr auto ARGB2BGRA 	= ZUAZO_CONSTRUCT_SWIZZLE( G, R, A, B );
	constexpr auto ARGB2GRBA 	= ZUAZO_CONSTRUCT_SWIZZLE( Identity, A, G, B );
	constexpr auto ARGB2GBRA 	= ZUAZO_CONSTRUCT_SWIZZLE( G, A, R, B );
	
	constexpr auto ABGR2XRGB 	= ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, R, One );
	constexpr auto ABGR2XBGR 	= ZUAZO_CONSTRUCT_SWIZZLE( Identity, Identity, Identity, One );
	constexpr auto ABGR2XGRB 	= ZUAZO_CONSTRUCT_SWIZZLE( G, B, R, One );
	constexpr auto ABGR2XGBR 	= ZUAZO_CONSTRUCT_SWIZZLE( Identity, B, G, One );
	constexpr auto ABGR2RGBX 	= ZUAZO_CONSTRUCT_SWIZZLE( A, B, G, One );
	constexpr auto ABGR2BGRX 	= ZUAZO_CONSTRUCT_SWIZZLE( G, B, A, One );
	constexpr auto ABGR2GRBX 	= ZUAZO_CONSTRUCT_SWIZZLE( B, A, G, One );
	constexpr auto ABGR2GBRX 	= ZUAZO_CONSTRUCT_SWIZZLE( G, A, B, One );
	constexpr auto ABGR2ARGB 	= ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, R, Identity );
	constexpr auto ABGR2ABGR 	= IDENTITY;
	constexpr auto ABGR2AGRB 	= ZUAZO_CONSTRUCT_SWIZZLE( G, B, R, Identity );
	constexpr auto ABGR2AGBR 	= ZUAZO_CONSTRUCT_SWIZZLE( Identity, B, G, Identity );
	constexpr auto ABGR2RGBA 	= ZUAZO_CONSTRUCT_SWIZZLE( A, B, G, R );
	constexpr auto ABGR2BGRA 	= ZUAZO_CONSTRUCT_SWIZZLE( G, B, A, R );
	constexpr auto ABGR2GRBA 	= ZUAZO_CONSTRUCT_SWIZZLE( B, A, G, R );
	constexpr auto ABGR2GBRA 	= ZUAZO_CONSTRUCT_SWIZZLE( G, A, Identity, R );

	constexpr auto RGBA2XRGB 	= ZUAZO_CONSTRUCT_SWIZZLE( G, B, A, One );
	constexpr auto RGBA2XBGR 	= ZUAZO_CONSTRUCT_SWIZZLE( A, B, G, One );
	constexpr auto RGBA2XGRB 	= ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, A, One );
	constexpr auto RGBA2XGBR 	= ZUAZO_CONSTRUCT_SWIZZLE( A, Identity, Identity, One );
	constexpr auto RGBA2RGBX 	= ZUAZO_CONSTRUCT_SWIZZLE( Identity, Identity, Identity, One );
	constexpr auto RGBA2BGRX 	= ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, R, One );
	constexpr auto RGBA2GRBX 	= ZUAZO_CONSTRUCT_SWIZZLE( G, R, Identity, One );
	constexpr auto RGBA2GBRX 	= ZUAZO_CONSTRUCT_SWIZZLE( B, R, G, One );
	constexpr auto RGBA2ARGB 	= ZUAZO_CONSTRUCT_SWIZZLE( G, B, A, R );
	constexpr auto RGBA2ABGR 	= ZUAZO_CONSTRUCT_SWIZZLE( A, B, G, R );
	constexpr auto RGBA2AGRB 	= ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, A, R );
	constexpr auto RGBA2AGBR 	= ZUAZO_CONSTRUCT_SWIZZLE( A, Identity, Identity, R );
	constexpr auto RGBA2RGBA 	= IDENTITY;
	constexpr auto RGBA2BGRA 	= ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, R, Identity );
	constexpr auto RGBA2GRBA 	= ZUAZO_CONSTRUCT_SWIZZLE( G, R, Identity, Identity );
	constexpr auto RGBA2GBRA 	= ZUAZO_CONSTRUCT_SWIZZLE( B, R, G, Identity );

	constexpr auto BGRA2XRGB = ZUAZO_CONSTRUCT_SWIZZLE( G, R, A, One );
	constexpr auto BGRA2XBGR = ZUAZO_CONSTRUCT_SWIZZLE( A, R, G, One );
	constexpr auto BGRA2XGRB = ZUAZO_CONSTRUCT_SWIZZLE( Identity, Identity, A, One );
	constexpr auto BGRA2XGBR = ZUAZO_CONSTRUCT_SWIZZLE( A, Identity, R, One );
	constexpr auto BGRA2RGBX = ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, R, One );
	constexpr auto BGRA2BGRX = ZUAZO_CONSTRUCT_SWIZZLE( Identity, Identity, Identity, One );
	constexpr auto BGRA2GRBX = ZUAZO_CONSTRUCT_SWIZZLE( G, B, R, One );
	constexpr auto BGRA2GBRX = ZUAZO_CONSTRUCT_SWIZZLE( Identity, B, G, One );
	constexpr auto BGRA2ARGB = ZUAZO_CONSTRUCT_SWIZZLE( G, R, A, B );
	constexpr auto BGRA2ABGR = ZUAZO_CONSTRUCT_SWIZZLE( A, R, G, B );
	constexpr auto BGRA2AGRB = ZUAZO_CONSTRUCT_SWIZZLE( Identity, Identity, A, R );
	constexpr auto BGRA2AGBR = ZUAZO_CONSTRUCT_SWIZZLE( A, Identity, R, B );
	constexpr auto BGRA2RGBA = ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, R, Identity );
	constexpr auto BGRA2BGRA = IDENTITY;
	constexpr auto BGRA2GRBA = ZUAZO_CONSTRUCT_SWIZZLE( G, B, R, Identity );
	constexpr auto BGRA2GBRA = ZUAZO_CONSTRUCT_SWIZZLE( Identity, B, G, Identity );

	constexpr auto RGB2BGR = ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, R, Identity );
	constexpr auto BGR2RGB = RGB2BGR;

	//Decide the equivalent
	switch (enc){
	case ColorEncoding::UINT:
		switch(fmt){
		//4 bit compoents
		case PixelFormat::Y4X4_8: 			return { vk::Format::eR4G4UnormPack8, RG2YX };
		case PixelFormat::X4Y4_8:			return { vk::Format::eR4G4UnormPack8, RG2XY };
		case PixelFormat::Y4A4_8: 			return { vk::Format::eR4G4UnormPack8, RG2YA };
		case PixelFormat::A4Y4_8:			return { vk::Format::eR4G4UnormPack8, RG2AY };

		case PixelFormat::X4R4G4B4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2XRGB };
		case PixelFormat::X4B4G4R4_16:		return { vk::Format::eR4G4B4A4UnormPack16,RGBA2XBGR };
		case PixelFormat::X4G4R4B4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2XRGB };
		case PixelFormat::X4G4B4R4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2XGBR };
		case PixelFormat::R4G4B4X4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2RGBX };
		case PixelFormat::B4G4R4X4_16:		return { vk::Format::eB4G4R4A4UnormPack16, BGRA2BGRX };
		case PixelFormat::G4R4B4X4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GRBX };
		case PixelFormat::G4B4R4X4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GBRX };

		case PixelFormat::A4R4G4B4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2ARGB };
		case PixelFormat::A4B4G4R4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2ABGR };
		case PixelFormat::A4G4R4B4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2ARGB };
		case PixelFormat::A4G4B4R4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2AGBR };
		case PixelFormat::R4G4B4A4_16:		return { vk::Format::eR4G4B4A4UnormPack16, IDENTITY };
		case PixelFormat::B4G4R4A4_16:		return { vk::Format::eB4G4R4A4UnormPack16, IDENTITY };
		case PixelFormat::G4R4B4A4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GRBA };
		case PixelFormat::G4B4R4A4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GBRA };

		//5 bit components
		case PixelFormat::X1R5G5B5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XRGB };
		case PixelFormat::X1B5G5R5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XBGR };
		case PixelFormat::X1G5R5B5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XGRB };
		case PixelFormat::X1G5B5R5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XGBR };
		case PixelFormat::R5G5B5X1_16:		return { vk::Format::eR5G5B5A1UnormPack16, RGBA2RGBX };
		case PixelFormat::B5G5R5X1_16:		return { vk::Format::eB5G5R5A1UnormPack16, BGRA2BGRX };
		case PixelFormat::G5R5B5X1_16:		return { vk::Format::eR5G5B5A1UnormPack16, RGBA2GRBX };
		case PixelFormat::G5B5R5X1_16:		return { vk::Format::eB5G5R5A1UnormPack16, RGBA2GBRX };

		case PixelFormat::A1R5G5B5_16:		return { vk::Format::eA1R5G5B5UnormPack16, IDENTITY };
		case PixelFormat::A1B5G5R5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2ABGR };
		case PixelFormat::A1G5R5B5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2AGRB };
		case PixelFormat::A1G5B5R5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2AGBR };
		case PixelFormat::R5G5B5A1_16:		return { vk::Format::eR5G5B5A1UnormPack16, IDENTITY };
		case PixelFormat::B5G5R5A1_16:		return { vk::Format::eB5G5R5A1UnormPack16, IDENTITY };
		case PixelFormat::G5R5B5A1_16:		return { vk::Format::eR5G5B5A1UnormPack16, RGBA2GRBA };
		case PixelFormat::G5B5R5A1_16:		return { vk::Format::eB5G5R5A1UnormPack16, RGBA2GBRA };

		//6 bit components
		case PixelFormat::R5G6B5_16:		return { vk::Format::eR5G6B5UnormPack16, IDENTITY };
		case PixelFormat::B5G6R5_16:		return { vk::Format::eB5G6R5UnormPack16, IDENTITY };

		//8 bit components
		case PixelFormat::Y8:				return { vk::Format::eR8Unorm, R2Y };

		case PixelFormat::Y8A8:				return { vk::Format::eR8G8Unorm, RG2YA };
		case PixelFormat::A8Y8:				return { vk::Format::eR8G8Unorm, RG2AY };

		case PixelFormat::R8G8B8:			return { vk::Format::eR8G8B8Unorm, IDENTITY };
		case PixelFormat::B8G8R8:			return { vk::Format::eB8G8R8Unorm, IDENTITY };
		case PixelFormat::G8R8B8:			return { vk::Format::eR8G8B8Unorm, RGBA2GRBA };
		case PixelFormat::G8B8R8:			return { vk::Format::eR8G8B8Unorm, RGBA2GBRA };

		case PixelFormat::A8R8G8B8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2ARGB };
		case PixelFormat::A8B8G8R8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2ABGR };
		case PixelFormat::A8G8R8B8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2AGRB };
		case PixelFormat::A8G8B8R8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2AGBR };
		case PixelFormat::R8G8B8A8:			return { vk::Format::eR8G8B8A8Unorm, IDENTITY };
		case PixelFormat::B8G8R8A8:			return { vk::Format::eB8G8R8A8Unorm, IDENTITY };
		case PixelFormat::G8R8B8A8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2GRBA };
		case PixelFormat::G8B8R8A8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2GBRA };

		#if ZUAZO_IS_BIG_ENDIAN
		case PixelFormat::X8R8G8B8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2XRGB };
		case PixelFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2XBGR };
		case PixelFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2RGBX };
		case PixelFormat::B8G8R8X8_32:		return { vk::Format::eB8G8R8A8Unorm, BGRA2BGRX };

		case PixelFormat::A8R8G8B8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2ARGB };
		case PixelFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2ABGR };
		case PixelFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Unorm, IDENTITY };
		case PixelFormat::B8G8R8A8_32:		return { vk::Format::eB8G8R8A8Unorm, IDENTITY };
		#elif ZUAZO_IS_LITTLE_ENDIAN
		case PixelFormat::X8R8G8B8_32:		return { vk::Format::eB8G8R8A8Unorm, BGRA2BGRX };
		case PixelFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2RGBX };
		case PixelFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2XBGR };
		case PixelFormat::B8G8R8X8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2XRGB };

		case PixelFormat::A8R8G8B8_32:		return { vk::Format::eB8G8R8A8Unorm, IDENTITY };
		case PixelFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Unorm, IDENTITY };
		case PixelFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2ABGR };
		case PixelFormat::B8G8R8A8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2ARGB };
		#endif

		case PixelFormat::R8G8B8G8_S422:	return { vk::Format::eB8G8R8G8422Unorm, BGR2RGB };
		case PixelFormat::B8G8R8G8_S422:	return { vk::Format::eB8G8R8G8422Unorm, IDENTITY };
		case PixelFormat::G8R8G8B8_S422:	return { vk::Format::eG8B8G8R8422Unorm, BGR2RGB };
		case PixelFormat::G8B8G8R8_S422:	return { vk::Format::eG8B8G8R8422Unorm, IDENTITY };

		case PixelFormat::G8_B8_R8:			return { vk::Format::eG8B8R83Plane444Unorm, IDENTITY };
		case PixelFormat::G8_B8_R8_S422:	return { vk::Format::eG8B8R83Plane422Unorm, IDENTITY };
		case PixelFormat::G8_B8_R8_S420:	return { vk::Format::eG8B8R83Plane420Unorm, IDENTITY };

		case PixelFormat::G8_R8B8_S422:		return { vk::Format::eG8B8R82Plane422Unorm, RGBA2BGRA };
		case PixelFormat::G8_B8R8_S422:		return { vk::Format::eG8B8R82Plane422Unorm, IDENTITY };
		case PixelFormat::G8_R8B8_S420:		return { vk::Format::eG8B8R82Plane420Unorm, RGBA2BGRA };
		case PixelFormat::G8_B8R8_S420:		return { vk::Format::eG8B8R82Plane420Unorm, IDENTITY };

		//10 bit components
		case PixelFormat::Y10X6_16:			return { vk::Format::eR10X6UnormPack16, R2Y };

		case PixelFormat::Y10X6A10X6_16:	return { vk::Format::eR10X6G10X6Unorm2Pack16, RG2YA };
		case PixelFormat::A10X6Y10X6_16:	return { vk::Format::eR10X6G10X6Unorm2Pack16, RG2AY };

		case PixelFormat::A10X6R10X6G10X6B10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2ARGB };
		case PixelFormat::A10X6B10X6G10X6R10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2ABGR };
		case PixelFormat::A10X6G10X6R10X6B10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2AGRB };
		case PixelFormat::A10X6G10X6B10X6R10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2AGBR };
		case PixelFormat::R10X6G10X6B10X6A10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, IDENTITY };
		case PixelFormat::B10X6G10X6R10X6A10X6_16:	return { vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16, IDENTITY };
		case PixelFormat::G10X6R10X6B10X6A10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2GRBA };
		case PixelFormat::G10X6B10X6R10X6A10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2GBRA };

		case PixelFormat::X2R10G10B10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XRGB };
		case PixelFormat::X2B10G10R10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XBGR };
		case PixelFormat::X2G10R10B10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XGRB };
		case PixelFormat::X2G10B10R10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XGBR };

		case PixelFormat::A2R10G10B10_32:	return { vk::Format::eA2R10G10B10UnormPack32, IDENTITY };
		case PixelFormat::A2B10G10R10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2ABGR };
		case PixelFormat::A2G10R10B10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2AGRB };
		case PixelFormat::A2G10B10R10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2AGBR };

		case PixelFormat::R10X6G10X6B10X6G10X6_S422_16:	return { vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16, BGR2RGB };
		case PixelFormat::B10X6G10X6R10X6G10X6_S422_16:	return { vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16, IDENTITY };
		case PixelFormat::G10X6R10X6G10X6B10X6_S422_16:	return { vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16, BGR2RGB };
		case PixelFormat::G10X6B10X6G10X6R10X6_S422_16:	return { vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16, IDENTITY };

		case PixelFormat::G10X6_B10X6_R10X6_16:			return { vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16, IDENTITY };
		case PixelFormat::G10X6_B10X6_R10X6_S422_16:	return { vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16, IDENTITY };
		case PixelFormat::G10X6_B10X6_R10X6_S420_16:	return { vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16, IDENTITY };

		case PixelFormat::G10X6_R10X6B10X6_S422_16:		return { vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16, RGBA2BGRA };
		case PixelFormat::G10X6_B10X6R10X6_S422_16:		return { vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16, IDENTITY };
		case PixelFormat::G10X6_R10X6B10X6_S420_16:		return { vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16, RGBA2BGRA };
		case PixelFormat::G10X6_B10X6R10X6_S420_16:		return { vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16, IDENTITY };

		//12 bits per component
		case PixelFormat::Y12X4_16:			return { vk::Format::eR12X4UnormPack16, R2Y };

		case PixelFormat::Y12X4A12X4_16:	return { vk::Format::eR12X4G12X4Unorm2Pack16, RG2YA };
		case PixelFormat::A12X4Y12X4_16:	return { vk::Format::eR12X4G12X4Unorm2Pack16, RG2AY };

		case PixelFormat::A12X4R12X4G12X4B12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2ARGB };
		case PixelFormat::A12X4B12X4G12X4R12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2ABGR };
		case PixelFormat::A12X4G12X4R12X4B12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2AGRB };
		case PixelFormat::A12X4G12X4B12X4R12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2AGBR };
		case PixelFormat::R12X4G12X4B12X4A12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, IDENTITY };
		case PixelFormat::B12X4G12X4R12X4A12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2BGRA };
		case PixelFormat::G12X4R12X4B12X4A12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2GRBA };
		case PixelFormat::G12X4B12X4R12X4A12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2GBRA };

		case PixelFormat::R12X4G12X4B12X4G12X4_S422_16:	return { vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16, BGR2RGB };
		case PixelFormat::B12X4G12X4R12X4G12X4_S422_16:	return { vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16, IDENTITY };
		case PixelFormat::G12X4R12X4G12X4B12X4_S422_16:	return { vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16, BGR2RGB };
		case PixelFormat::G12X4B12X4G12X4R12X4_S422_16:	return { vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16, IDENTITY };

		case PixelFormat::G12X4_B12X4_R12X4_16:			return { vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16, IDENTITY };
		case PixelFormat::G12X4_B12X4_R12X4_S422_16:	return { vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16, IDENTITY };
		case PixelFormat::G12X4_B12X4_R12X4_S420_16:	return { vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16, IDENTITY };

		case PixelFormat::G12X4_R12X4B12X4_S422_16:		return { vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16, RGBA2BGRA };
		case PixelFormat::G12X4_B12X4R12X4_S422_16:		return { vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16, IDENTITY };
		case PixelFormat::G12X4_R12X4B12X4_S420_16:		return { vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16, RGBA2BGRA };
		case PixelFormat::G12X4_B12X4R12X4_S420_16:		return { vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16, IDENTITY };


		//16 bits per component
		case PixelFormat::Y16:				return { vk::Format::eR16Unorm, R2Y };

		case PixelFormat::Y16A16:			return { vk::Format::eR16G16Unorm, RG2YA };
		case PixelFormat::A16Y16:			return { vk::Format::eR16G16Unorm, RG2AY };
			
		case PixelFormat::R16G16B16:		return { vk::Format::eR16G16B16Unorm, IDENTITY };
		case PixelFormat::B16G16R16:		return { vk::Format::eR16G16B16Unorm, RGBA2BGRA };
		case PixelFormat::G16R16B16:		return { vk::Format::eR16G16B16Unorm, RGBA2GRBA };
		case PixelFormat::G16B16R16:		return { vk::Format::eR16G16B16Unorm, RGBA2GBRA };

		case PixelFormat::A16R16G16B16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2ARGB };
		case PixelFormat::A16B16G16R16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2ABGR };
		case PixelFormat::A16G16R16B16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2AGRB };
		case PixelFormat::A16G16B16R16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2AGBR };
		case PixelFormat::R16G16B16A16:		return { vk::Format::eR16G16B16A16Unorm, IDENTITY };
		case PixelFormat::B16G16R16A16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2BGRA };
		case PixelFormat::G16R16B16A16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2GRBA };
		case PixelFormat::G16B16R16A16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2GBRA };

		case PixelFormat::R16G16B16G16_S422:	return { vk::Format::eB16G16R16G16422Unorm, BGR2RGB };
		case PixelFormat::B16G16R16G16_S422:	return { vk::Format::eB16G16R16G16422Unorm, IDENTITY };
		case PixelFormat::G16R16G16B16_S422:	return { vk::Format::eG16B16G16R16422Unorm, BGR2RGB };
		case PixelFormat::G16B16G16R16_S422:	return { vk::Format::eG16B16G16R16422Unorm, IDENTITY };

		case PixelFormat::G16_B16_R16:		return { vk::Format::eG16B16R163Plane444Unorm, IDENTITY };
		case PixelFormat::G16_B16_R16_S422:	return { vk::Format::eG16B16R163Plane422Unorm, IDENTITY };
		case PixelFormat::G16_B16_R16_S420:	return { vk::Format::eG16B16R163Plane420Unorm, IDENTITY };

		case PixelFormat::G16_R16B16_S422:	return { vk::Format::eG16B16R162Plane422Unorm, RGBA2BGRA };
		case PixelFormat::G16_B16R16_S422:	return { vk::Format::eG16B16R162Plane422Unorm, IDENTITY };
		case PixelFormat::G16_R16B16_S420:	return { vk::Format::eG16B16R162Plane420Unorm, RGBA2BGRA };
		case PixelFormat::G16_B16R16_S420:	return { vk::Format::eG16B16R162Plane420Unorm, IDENTITY };

		default: return {};
		}

	case ColorEncoding::INT:
		switch(fmt){
		//8 bit components
		case PixelFormat::Y8:				return { vk::Format::eR8Snorm, R2Y };

		case PixelFormat::Y8A8:				return { vk::Format::eR8G8Snorm, RG2YA };
		case PixelFormat::A8Y8:				return { vk::Format::eR8G8Snorm, RG2AY };

		case PixelFormat::R8G8B8:			return { vk::Format::eR8G8B8Snorm, IDENTITY };
		case PixelFormat::B8G8R8:			return { vk::Format::eB8G8R8Snorm, IDENTITY };
		case PixelFormat::G8R8B8:			return { vk::Format::eR8G8B8Snorm, RGBA2GRBA };
		case PixelFormat::G8B8R8:			return { vk::Format::eR8G8B8Snorm, RGBA2GBRA };

		case PixelFormat::A8R8G8B8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2ARGB };
		case PixelFormat::A8B8G8R8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2ABGR };
		case PixelFormat::A8G8R8B8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2AGRB };
		case PixelFormat::A8G8B8R8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2AGBR };
		case PixelFormat::R8G8B8A8:			return { vk::Format::eR8G8B8A8Snorm, IDENTITY };
		case PixelFormat::B8G8R8A8:			return { vk::Format::eB8G8R8A8Snorm, IDENTITY };
		case PixelFormat::G8R8B8A8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2GRBA };
		case PixelFormat::G8B8R8A8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2GBRA };

		#if ZUAZO_IS_BIG_ENDIAN
		case PixelFormat::X8R8G8B8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2XRGB };
		case PixelFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2XBGR };
		case PixelFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2RGBX };
		case PixelFormat::B8G8R8X8_32:		return { vk::Format::eB8G8R8A8Snorm, BGRA2BGRX };

		case PixelFormat::A8R8G8B8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2ARGB };
		case PixelFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2ABGR };
		case PixelFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Snorm, IDENTITY };
		case PixelFormat::B8G8R8A8_32:		return { vk::Format::eB8G8R8A8Snorm, IDENTITY };
		#elif ZUAZO_IS_LITTLE_ENDIAN
		case PixelFormat::X8R8G8B8_32:		return { vk::Format::eB8G8R8A8Snorm, BGRA2BGRX };
		case PixelFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2RGBX };
		case PixelFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2XBGR };
		case PixelFormat::B8G8R8X8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2XRGB };

		case PixelFormat::A8R8G8B8_32:		return { vk::Format::eB8G8R8A8Snorm, IDENTITY };
		case PixelFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Snorm, IDENTITY };
		case PixelFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2ABGR };
		case PixelFormat::B8G8R8A8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2ARGB };
		#endif

		//16 bits per component
		case PixelFormat::Y16:				return { vk::Format::eR16Snorm, R2Y };

		case PixelFormat::Y16A16:			return { vk::Format::eR16G16Snorm, RG2YA };
		case PixelFormat::A16Y16:			return { vk::Format::eR16G16Snorm, RG2AY };
			
		case PixelFormat::R16G16B16:		return { vk::Format::eR16G16B16Snorm, IDENTITY };
		case PixelFormat::B16G16R16:		return { vk::Format::eR16G16B16Snorm, RGBA2BGRA };
		case PixelFormat::G16R16B16:		return { vk::Format::eR16G16B16Snorm, RGBA2GRBA };
		case PixelFormat::G16B16R16:		return { vk::Format::eR16G16B16Snorm, RGBA2GBRA };

		case PixelFormat::A16R16G16B16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2ARGB };
		case PixelFormat::A16B16G16R16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2ABGR };
		case PixelFormat::A16G16R16B16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2AGRB };
		case PixelFormat::A16G16B16R16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2AGBR };
		case PixelFormat::R16G16B16A16:		return { vk::Format::eR16G16B16A16Snorm, IDENTITY };
		case PixelFormat::B16G16R16A16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2BGRA };
		case PixelFormat::G16R16B16A16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2GRBA };
		case PixelFormat::G16B16R16A16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2GBRA };

		default: return {};
		}
	case ColorEncoding::IEEE754:
		switch(fmt){
		//16 bits per component
		case PixelFormat::Y16:				return { vk::Format::eR16Sfloat, R2Y };

		case PixelFormat::Y16A16:			return { vk::Format::eR16G16Sfloat, RG2YA };
		case PixelFormat::A16Y16:			return { vk::Format::eR16G16Sfloat, RG2AY };
			
		case PixelFormat::R16G16B16:		return { vk::Format::eR16G16B16Sfloat, IDENTITY };
		case PixelFormat::B16G16R16:		return { vk::Format::eR16G16B16Sfloat, RGBA2BGRA };
		case PixelFormat::G16R16B16:		return { vk::Format::eR16G16B16Sfloat, RGBA2GRBA };
		case PixelFormat::G16B16R16:		return { vk::Format::eR16G16B16Sfloat, RGBA2GBRA };

		case PixelFormat::A16R16G16B16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2ARGB };
		case PixelFormat::A16B16G16R16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2ABGR };
		case PixelFormat::A16G16R16B16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2AGRB };
		case PixelFormat::A16G16B16R16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2AGBR };
		case PixelFormat::R16G16B16A16:		return { vk::Format::eR16G16B16A16Sfloat, IDENTITY };
		case PixelFormat::B16G16R16A16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2BGRA };
		case PixelFormat::G16R16B16A16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2GRBA };
		case PixelFormat::G16B16R16A16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2GBRA };

		//32 bits per component
		case PixelFormat::Y32:				return { vk::Format::eR32Sfloat, R2Y };

		case PixelFormat::Y32A32:			return { vk::Format::eR32G32Sfloat, RG2YA };
		case PixelFormat::A32Y32:			return { vk::Format::eR32G32Sfloat, RG2AY };
			
		case PixelFormat::R32G32B32:		return { vk::Format::eR32G32B32Sfloat, IDENTITY };
		case PixelFormat::B32G32R32:		return { vk::Format::eR32G32B32Sfloat, RGBA2BGRA };
		case PixelFormat::G32R32B32:		return { vk::Format::eR32G32B32Sfloat, RGBA2GRBA };
		case PixelFormat::G32B32R32:		return { vk::Format::eR32G32B32Sfloat, RGBA2GBRA };

		case PixelFormat::A32R32G32B32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2ARGB };
		case PixelFormat::A32B32G32R32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2ABGR };
		case PixelFormat::A32G32R32B32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2AGRB };
		case PixelFormat::A32G32B32R32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2AGBR };
		case PixelFormat::R32G32B32A32:		return { vk::Format::eR32G32B32A32Sfloat, IDENTITY };
		case PixelFormat::B32G32R32A32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2BGRA };
		case PixelFormat::G32R32B32A32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2GRBA };
		case PixelFormat::G32B32R32A32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2GBRA };

		//64 bits per component
		case PixelFormat::Y64:				return { vk::Format::eR64Sfloat, R2Y };

		case PixelFormat::Y64A64:			return { vk::Format::eR64G64Sfloat, RG2YA };
		case PixelFormat::A64Y64:			return { vk::Format::eR64G64Sfloat, RG2AY };
			
		case PixelFormat::R64G64B64:		return { vk::Format::eR64G64B64Sfloat, IDENTITY };
		case PixelFormat::B64G64R64:		return { vk::Format::eR64G64B64Sfloat, RGBA2BGRA };
		case PixelFormat::G64R64B64:		return { vk::Format::eR64G64B64Sfloat, RGBA2GRBA };
		case PixelFormat::G64B64R64:		return { vk::Format::eR64G64B64Sfloat, RGBA2GBRA };

		case PixelFormat::A64R64G64B64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2ARGB };
		case PixelFormat::A64B64G64R64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2ABGR };
		case PixelFormat::A64G64R64B64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2AGRB };
		case PixelFormat::A64G64B64R64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2AGBR };
		case PixelFormat::R64G64B64A64:		return { vk::Format::eR64G64B64A64Sfloat, IDENTITY };
		case PixelFormat::B64G64R64A64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2BGRA };
		case PixelFormat::G64R64B64A64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2GRBA };
		case PixelFormat::G64B64R64A64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2GBRA };

		default: return {};
		}
	case ColorEncoding::IEC61966_2_1:
		switch(fmt){
		//8 bit components
		case PixelFormat::Y8:				return { vk::Format::eR8Srgb, R2Y };

		case PixelFormat::Y8A8:				return { vk::Format::eR8G8Srgb, RG2YA };
		case PixelFormat::A8Y8:				return { vk::Format::eR8G8Srgb, RG2AY };

		case PixelFormat::R8G8B8:			return { vk::Format::eR8G8B8Srgb, IDENTITY };
		case PixelFormat::B8G8R8:			return { vk::Format::eB8G8R8Srgb, IDENTITY };
		case PixelFormat::G8R8B8:			return { vk::Format::eR8G8B8Srgb, RGBA2GRBA };
		case PixelFormat::G8B8R8:			return { vk::Format::eR8G8B8Srgb, RGBA2GBRA };

		case PixelFormat::A8R8G8B8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2ARGB };
		case PixelFormat::A8B8G8R8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2ABGR };
		case PixelFormat::A8G8R8B8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2AGRB };
		case PixelFormat::A8G8B8R8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2AGBR };
		case PixelFormat::R8G8B8A8:			return { vk::Format::eR8G8B8A8Srgb, IDENTITY };
		case PixelFormat::B8G8R8A8:			return { vk::Format::eB8G8R8A8Srgb, IDENTITY };
		case PixelFormat::G8R8B8A8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2GRBA };
		case PixelFormat::G8B8R8A8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2GBRA };

		#if ZUAZO_IS_BIG_ENDIAN
		case PixelFormat::X8R8G8B8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2XRGB };
		case PixelFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2XBGR };
		case PixelFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2RGBX };
		case PixelFormat::B8G8R8X8_32:		return { vk::Format::eB8G8R8A8Srgb, BGRA2BGRX };

		case PixelFormat::A8R8G8B8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2ARGB };
		case PixelFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2ABGR };
		case PixelFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Srgb, IDENTITY };
		case PixelFormat::B8G8R8A8_32:		return { vk::Format::eB8G8R8A8Srgb, IDENTITY };
		#elif ZUAZO_IS_LITTLE_ENDIAN
		case PixelFormat::X8R8G8B8_32:		return { vk::Format::eB8G8R8A8Srgb, BGRA2BGRX };
		case PixelFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2RGBX };
		case PixelFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2XBGR };
		case PixelFormat::B8G8R8X8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2XRGB };

		case PixelFormat::A8R8G8B8_32:		return { vk::Format::eB8G8R8A8Srgb, IDENTITY };
		case PixelFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Srgb, IDENTITY };
		case PixelFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2ABGR };
		case PixelFormat::B8G8R8A8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2ARGB };
		#endif

		default: return {};
		}
	default: return {};
	}
}

constexpr std::tuple<PixelFormat, ColorEncoding> fromVulkan(vk::Format fmt){
	switch(fmt){
	//4 bit formats
	case vk::Format::eR4G4B4A4UnormPack16: 	return { PixelFormat::R4G4B4A4_16, ColorEncoding::UINT };
	case vk::Format::eB4G4R4A4UnormPack16: 	return { PixelFormat::B4G4R4A4_16, ColorEncoding::UINT };

	//5 bits formats
	case vk::Format::eR5G5B5A1UnormPack16: 	return { PixelFormat::R5G5B5A1_16, ColorEncoding::UINT };
	case vk::Format::eB5G5R5A1UnormPack16: 	return { PixelFormat::B5G5R5A1_16, ColorEncoding::UINT };
	case vk::Format::eA1R5G5B5UnormPack16: 	return { PixelFormat::A1R5G5B5_16, ColorEncoding::UINT };

	//6 bit formats
	case vk::Format::eR5G6B5UnormPack16:	return { PixelFormat::R5G6B5_16, ColorEncoding::UINT };
	case vk::Format::eB5G6R5UnormPack16:	return { PixelFormat::B5G6R5_16, ColorEncoding::UINT };

	//8bit formats
	case vk::Format::eR8G8B8Unorm:			return { PixelFormat::R8G8B8, ColorEncoding::UINT };
	case vk::Format::eR8G8B8Snorm:			return { PixelFormat::R8G8B8, ColorEncoding::INT };
	case vk::Format::eR8G8B8Srgb:			return { PixelFormat::R8G8B8, ColorEncoding::IEC61966_2_1 };
	case vk::Format::eB8G8R8Unorm:			return { PixelFormat::B8G8R8, ColorEncoding::UINT };
	case vk::Format::eB8G8R8Snorm:			return { PixelFormat::B8G8R8, ColorEncoding::INT };
	case vk::Format::eB8G8R8Srgb:			return { PixelFormat::B8G8R8, ColorEncoding::IEC61966_2_1 };
	case vk::Format::eR8G8B8A8Unorm:		return { PixelFormat::R8G8B8A8, ColorEncoding::UINT };
	case vk::Format::eR8G8B8A8Snorm:		return { PixelFormat::R8G8B8A8, ColorEncoding::INT };
	case vk::Format::eR8G8B8A8Srgb:			return { PixelFormat::R8G8B8A8, ColorEncoding::IEC61966_2_1 };
	case vk::Format::eB8G8R8A8Unorm:		return { PixelFormat::B8G8R8A8, ColorEncoding::UINT };
	case vk::Format::eB8G8R8A8Snorm:		return { PixelFormat::B8G8R8A8, ColorEncoding::INT };
	case vk::Format::eB8G8R8A8Srgb:			return { PixelFormat::B8G8R8A8, ColorEncoding::IEC61966_2_1 };
	case vk::Format::eG8B8G8R8422Unorm:		return { PixelFormat::G8B8G8R8_S422, ColorEncoding::UINT };
	case vk::Format::eB8G8R8G8422Unorm:		return { PixelFormat::B8G8R8G8_S422, ColorEncoding::UINT };
	case vk::Format::eG8B8R83Plane420Unorm:	return { PixelFormat::G8_B8_R8_S420, ColorEncoding::UINT };
	case vk::Format::eG8B8R83Plane422Unorm:	return { PixelFormat::G8_B8_R8_S422, ColorEncoding::UINT };
	case vk::Format::eG8B8R83Plane444Unorm:	return { PixelFormat::G8_B8_R8, ColorEncoding::UINT };
	case vk::Format::eG8B8R82Plane420Unorm:	return { PixelFormat::G8_B8R8_S420, ColorEncoding::UINT };
	case vk::Format::eG8B8R82Plane422Unorm:	return { PixelFormat::G8_B8R8_S422, ColorEncoding::UINT };

	//10 bit formats
	case vk::Format::eA2R10G10B10UnormPack32:	return { PixelFormat::A2R10G10B10_32, ColorEncoding::UINT };
	case vk::Format::eA2R10G10B10SnormPack32:	return { PixelFormat::A2R10G10B10_32, ColorEncoding::INT };
	case vk::Format::eA2B10G10R10UnormPack32:	return { PixelFormat::A2B10G10R10_32, ColorEncoding::UINT };
	case vk::Format::eA2B10G10R10SnormPack32:	return { PixelFormat::A2B10G10R10_32, ColorEncoding::INT };
	case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16:		return { PixelFormat::R10X6G10X6B10X6A10X6_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16:	return { PixelFormat::G10X6B10X6G10X6R10X6_S422_16, ColorEncoding::UINT };
	case vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16:	return { PixelFormat::B10X6G10X6R10X6G10X6_S422_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16:	return { PixelFormat::G10X6_B10X6_R10X6_S420_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16:	return { PixelFormat::G10X6_B10X6_R10X6_S422_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16:	return { PixelFormat::G10X6_B10X6_R10X6_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16:	return { PixelFormat::G10X6_B10X6R10X6_S420_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16:	return { PixelFormat::G10X6_B10X6R10X6_S422_16, ColorEncoding::UINT };

	//12 bit formats
	case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16:		return { PixelFormat::R12X4G12X4B12X4A12X4_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16:	return { PixelFormat::G12X4B12X4G12X4R12X4_S422_16, ColorEncoding::UINT };
	case vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16:	return { PixelFormat::B12X4G12X4R12X4G12X4_S422_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16:	return { PixelFormat::G12X4_B12X4_R12X4_S420_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16:	return { PixelFormat::G12X4_B12X4_R12X4_S422_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16:	return { PixelFormat::G12X4_B12X4_R12X4_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16:	return { PixelFormat::G12X4_B12X4R12X4_S420_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16:	return { PixelFormat::G12X4_B12X4R12X4_S422_16, ColorEncoding::UINT };

	//16 bit formats
	case vk::Format::eR16G16B16Unorm:			return { PixelFormat::R16G16B16, ColorEncoding::UINT };
	case vk::Format::eR16G16B16Snorm:			return { PixelFormat::R16G16B16, ColorEncoding::INT };
	case vk::Format::eR16G16B16Sfloat:			return { PixelFormat::R16G16B16, ColorEncoding::IEEE754 };
	case vk::Format::eR16G16B16A16Unorm:		return { PixelFormat::R16G16B16A16, ColorEncoding::UINT };
	case vk::Format::eR16G16B16A16Snorm:		return { PixelFormat::R16G16B16A16, ColorEncoding::INT };
	case vk::Format::eR16G16B16A16Sfloat:		return { PixelFormat::R16G16B16A16, ColorEncoding::IEEE754 };
	case vk::Format::eG16B16G16R16422Unorm:		return { PixelFormat::G16B16G16R16_S422, ColorEncoding::UINT };
	case vk::Format::eB16G16R16G16422Unorm:		return { PixelFormat::B16G16R16G16_S422, ColorEncoding::UINT };
	case vk::Format::eG16B16R163Plane420Unorm:	return { PixelFormat::G16_B16_R16_S420, ColorEncoding::UINT };
	case vk::Format::eG16B16R163Plane422Unorm:	return { PixelFormat::G16_B16_R16_S422, ColorEncoding::UINT };
	case vk::Format::eG16B16R163Plane444Unorm:	return { PixelFormat::G16_B16_R16, ColorEncoding::UINT };
	case vk::Format::eG16B16R162Plane420Unorm:	return { PixelFormat::G16_B16R16_S420, ColorEncoding::UINT };
	case vk::Format::eG16B16R162Plane422Unorm:	return { PixelFormat::G16_B16R16_S422, ColorEncoding::UINT };

	//32 bit formats
	case vk::Format::eR32G32B32Sfloat:		return { PixelFormat::R32G32B32, ColorEncoding::IEEE754 };
	case vk::Format::eR32G32B32A32Sfloat:	return { PixelFormat::R32G32B32A32, ColorEncoding::IEEE754 };

	//64 bit formats
	case vk::Format::eR64G64B64Sfloat:		return { PixelFormat::R64G64B64, ColorEncoding::IEEE754 };
	case vk::Format::eR64G64B64A64Sfloat:	return { PixelFormat::R64G64B64A64, ColorEncoding::IEEE754 };

	default: return {};
	}
}

constexpr vk::ColorSpaceKHR toVulkan(ColorPrimaries prim, ColorEncoding enc){
	constexpr vk::ColorSpaceKHR ERROR = static_cast<vk::ColorSpaceKHR>(-1);

	switch(enc){
	case ColorEncoding::UINT:
	case ColorEncoding::INT:
	case ColorEncoding::IEEE754:
		//Linear encoding
		switch(prim){
		case ColorPrimaries::BT709: 	return vk::ColorSpaceKHR::eBt709LinearEXT;
		case ColorPrimaries::BT2020:	return vk::ColorSpaceKHR::eBt2020LinearEXT;
		case ColorPrimaries::SMPTE432:	return vk::ColorSpaceKHR::eDisplayP3LinearEXT;
		case ColorPrimaries::ADOBE_RGB:	return vk::ColorSpaceKHR::eAdobergbLinearEXT;
		default: return ERROR;
		}
	case ColorEncoding::IEC61966_2_1:
		//IEC61966_2_1 non linear encoding
		switch(prim){
		case ColorPrimaries::BT709: 	return vk::ColorSpaceKHR::eSrgbNonlinear;
		case ColorPrimaries::SMPTE432:	return vk::ColorSpaceKHR::eDisplayP3NonlinearEXT;
		default: return ERROR;
		}
	default: return ERROR;
	}
}

constexpr std::tuple<ColorPrimaries, ColorEncoding> fromVulkan(vk::ColorSpaceKHR space){
	switch(space){
	case vk::ColorSpaceKHR::eBt709LinearEXT: return { ColorPrimaries::BT709, ColorEncoding::NONE };
	case vk::ColorSpaceKHR::eBt2020LinearEXT: return { ColorPrimaries::BT2020, ColorEncoding::NONE };
	case vk::ColorSpaceKHR::eDisplayP3LinearEXT: return { ColorPrimaries::SMPTE432, ColorEncoding::NONE };
	case vk::ColorSpaceKHR::eAdobergbLinearEXT: return { ColorPrimaries::ADOBE_RGB, ColorEncoding::NONE };
	case vk::ColorSpaceKHR::eSrgbNonlinear: return { ColorPrimaries::BT709, ColorEncoding::IEC61966_2_1 };
	case vk::ColorSpaceKHR::eDisplayP3NonlinearEXT: return { ColorPrimaries::SMPTE432, ColorEncoding::IEC61966_2_1 };
	default: return {};
	}
}

inline vk::SurfaceFormatKHR toVulkan(PixelFormat fmt, ColorPrimaries prim, ColorEncoding enc){
	const auto [format, swizzle] = toVulkan(fmt, enc);
	if(format == vk::Format::eUndefined || swizzle != vk::ComponentMapping()) return {};

	const vk::ColorSpaceKHR space = toVulkan(prim, enc);
	if(space == static_cast<vk::ColorSpaceKHR>(-1)) return {};
	
	return vk::SurfaceFormatKHR(
		format,
		space		
	);
}

constexpr std::tuple<PixelFormat, ColorPrimaries, ColorEncoding> fromVulkan(const vk::SurfaceFormatKHR& fmt){
	const auto[format, enc] = fromVulkan(fmt.format);
	if(format == PixelFormat::NONE || enc == ColorEncoding::NONE) return {};

	const auto[prim, enc2] = fromVulkan(fmt.colorSpace);
	if(prim == ColorPrimaries::NONE) return {};
	if(enc2 == ColorEncoding::NONE){
		//Linear encoding
		switch(enc){
		case ColorEncoding::UINT:
		case ColorEncoding::INT:
		case ColorEncoding::IEEE754:
			break;
		default: return {};
		}
	} else {
		//Non linear encoding
		if(enc2 != enc) return {};
	}

	return {format, prim, enc};
}


}
