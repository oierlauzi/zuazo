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

constexpr std::tuple<vk::Format, vk::ComponentMapping> toVulkan(ColorFormat fmt, ColorEncoding enc){
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
	//constexpr auto ARGB2ARGB 	= IDENTITY; //Ununsed
	constexpr auto ARGB2ABGR 	= ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, R, Identity );
	constexpr auto ARGB2AGRB 	= ZUAZO_CONSTRUCT_SWIZZLE( G, R, Identity, Identity );
	constexpr auto ARGB2AGBR 	= ZUAZO_CONSTRUCT_SWIZZLE( B, R, G, Identity );

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
	//constexpr auto RGBA2RGBA 	= IDENTITY; //Unused
	constexpr auto RGBA2BGRA 	= ZUAZO_CONSTRUCT_SWIZZLE( B, Identity, R, Identity );
	constexpr auto RGBA2GRBA 	= ZUAZO_CONSTRUCT_SWIZZLE( G, R, Identity, Identity );
	constexpr auto RGBA2GBRA 	= ZUAZO_CONSTRUCT_SWIZZLE( B, R, G, Identity );

	//Decide the equivalent
	switch (enc){
	case ColorEncoding::UINT:
		switch(fmt){
		//4 bit compoents
		case ColorFormat::Y4X4_8: 			return { vk::Format::eR4G4UnormPack8, RG2YX };
		case ColorFormat::X4Y4_8:			return { vk::Format::eR4G4UnormPack8, RG2XY };
		case ColorFormat::Y4A4_8: 			return { vk::Format::eR4G4UnormPack8, RG2YA };
		case ColorFormat::A4Y4_8:			return { vk::Format::eR4G4UnormPack8, RG2AY };

		case ColorFormat::X4R4G4B4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2XRGB };
		case ColorFormat::X4B4G4R4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2XBGR };
		case ColorFormat::X4G4R4B4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2XGRB };
		case ColorFormat::X4G4B4R4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2XGBR };
		case ColorFormat::R4G4B4X4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2RGBX };
		case ColorFormat::B4G4R4X4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2BGRX };
		case ColorFormat::G4R4B4X4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GRBX };
		case ColorFormat::G4B4R4X4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GBRX };

		case ColorFormat::A4R4G4B4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2ARGB };
		case ColorFormat::A4B4G4R4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2ABGR };
		case ColorFormat::A4G4R4B4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2AGRB };
		case ColorFormat::A4G4B4R4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2AGBR };
		case ColorFormat::R4G4B4A4_16:		return { vk::Format::eR4G4B4A4UnormPack16, IDENTITY };
		case ColorFormat::B4G4R4A4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2BGRA };
		case ColorFormat::G4R4B4A4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GRBA };
		case ColorFormat::G4B4R4A4_16:		return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GBRA };

		//5 bit components
		case ColorFormat::X1R5G5B5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XRGB };
		case ColorFormat::X1B5G5R5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XBGR };
		case ColorFormat::X1G5R5B5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XGRB };
		case ColorFormat::X1G5B5R5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XGBR };
		case ColorFormat::R5G5B5X1_16:		return { vk::Format::eR5G5B5A1UnormPack16, RGBA2RGBX };
		case ColorFormat::B5G5R5X1_16:		return { vk::Format::eR5G5B5A1UnormPack16, RGBA2BGRX };
		case ColorFormat::G5R5B5X1_16:		return { vk::Format::eR5G5B5A1UnormPack16, RGBA2GRBX };
		case ColorFormat::G5B5R5X1_16:		return { vk::Format::eR5G5B5A1UnormPack16, RGBA2GBRX };

		case ColorFormat::A1R5G5B5_16:		return { vk::Format::eA1R5G5B5UnormPack16, IDENTITY };
		case ColorFormat::A1B5G5R5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2ABGR };
		case ColorFormat::A1G5R5B5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2AGRB };
		case ColorFormat::A1G5B5R5_16:		return { vk::Format::eA1R5G5B5UnormPack16, ARGB2AGBR };
		case ColorFormat::R5G5B5A1_16:		return { vk::Format::eR5G5B5A1UnormPack16, IDENTITY };
		case ColorFormat::B5G5R5A1_16:		return { vk::Format::eR5G5B5A1UnormPack16, RGBA2BGRA };
		case ColorFormat::G5R5B5A1_16:		return { vk::Format::eR5G5B5A1UnormPack16, RGBA2GRBA };
		case ColorFormat::G5B5R5A1_16:		return { vk::Format::eR5G5B5A1UnormPack16, RGBA2GBRA };

		//6 bit components
		case ColorFormat::R5G6B5_16:		return { vk::Format::eR5G6B5UnormPack16, IDENTITY };
		case ColorFormat::B5G6R5_16:		return { vk::Format::eR5G6B5UnormPack16, RGBA2BGRA };

		//8 bit components
		case ColorFormat::Y8:				return { vk::Format::eR8Unorm, R2Y };

		case ColorFormat::Y8A8:				return { vk::Format::eR8G8Unorm, RG2YA };
		case ColorFormat::A8Y8:				return { vk::Format::eR8G8Unorm, RG2AY };

		case ColorFormat::R8G8B8:			return { vk::Format::eR8G8B8Unorm, IDENTITY };
		case ColorFormat::B8G8R8:			return { vk::Format::eR8G8B8Unorm, RGBA2BGRA };
		case ColorFormat::G8R8B8:			return { vk::Format::eR8G8B8Unorm, RGBA2GRBA };
		case ColorFormat::G8B8R8:			return { vk::Format::eR8G8B8Unorm, RGBA2GBRA };

		case ColorFormat::A8R8G8B8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2ARGB };
		case ColorFormat::A8B8G8R8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2ABGR };
		case ColorFormat::A8G8R8B8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2AGRB };
		case ColorFormat::A8G8B8R8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2AGBR };
		case ColorFormat::R8G8B8A8:			return { vk::Format::eR8G8B8A8Unorm, IDENTITY };
		case ColorFormat::B8G8R8A8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2BGRA };
		case ColorFormat::G8R8B8A8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2GRBA };
		case ColorFormat::G8B8R8A8:			return { vk::Format::eR8G8B8A8Unorm, RGBA2GBRA };

		#if ZUAZO_IS_BIG_ENDIAN
		case ColorFormat::X8R8G8B8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2XRGB };
		case ColorFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2XBGR };
		case ColorFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2RGBX };
		case ColorFormat::B8G8R8X8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2BGRX };

		case ColorFormat::A8R8G8B8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2ARGB };
		case ColorFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2ABGR };
		case ColorFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Unorm, IDENTITY };
		case ColorFormat::B8G8R8A8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2BGRA };
		#elif ZUAZO_IS_LITTLE_ENDIAN
		case ColorFormat::X8R8G8B8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2BGRX };
		case ColorFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2RGBX };
		case ColorFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2XBGR };
		case ColorFormat::B8G8R8X8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2XRGB };

		case ColorFormat::A8R8G8B8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2BGRA };
		case ColorFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Unorm, IDENTITY };
		case ColorFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2ABGR };
		case ColorFormat::B8G8R8A8_32:		return { vk::Format::eR8G8B8A8Unorm, RGBA2ARGB };
		#endif

		case ColorFormat::R8G8B8G8_S422:	return { vk::Format::eB8G8R8G8422Unorm, RGBA2BGRA };
		case ColorFormat::B8G8R8G8_S422:	return { vk::Format::eB8G8R8G8422Unorm, IDENTITY };
		case ColorFormat::G8R8G8B8_S422:	return { vk::Format::eG8B8G8R8422Unorm, RGBA2BGRA };
		case ColorFormat::G8B8G8R8_S422:	return { vk::Format::eG8B8G8R8422Unorm, IDENTITY };

		case ColorFormat::G8_B8_R8:			return { vk::Format::eG8B8R83Plane444Unorm, IDENTITY };
		case ColorFormat::G8_B8_R8_S422:	return { vk::Format::eG8B8R83Plane422Unorm, IDENTITY };
		case ColorFormat::G8_B8_R8_S420:	return { vk::Format::eG8B8R83Plane420Unorm, IDENTITY };

		case ColorFormat::G8_R8B8_S422:		return { vk::Format::eG8B8R82Plane422Unorm, RGBA2BGRA };
		case ColorFormat::G8_B8R8_S422:		return { vk::Format::eG8B8R82Plane422Unorm, IDENTITY };
		case ColorFormat::G8_R8B8_S420:		return { vk::Format::eG8B8R82Plane420Unorm, RGBA2BGRA };
		case ColorFormat::G8_B8R8_S420:		return { vk::Format::eG8B8R82Plane420Unorm, IDENTITY };

		//10 bit components
		case ColorFormat::Y10X6_16:			return { vk::Format::eR10X6UnormPack16, R2Y };

		case ColorFormat::Y10X6A10X6_16:	return { vk::Format::eR10X6G10X6Unorm2Pack16, RG2YA };
		case ColorFormat::A10X6Y10X6_16:	return { vk::Format::eR10X6G10X6Unorm2Pack16, RG2AY };

		case ColorFormat::A10X6R10X6G10X6B10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2ARGB };
		case ColorFormat::A10X6B10X6G10X6R10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2ABGR };
		case ColorFormat::A10X6G10X6R10X6B10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2AGRB };
		case ColorFormat::A10X6G10X6B10X6R10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2AGBR };
		case ColorFormat::R10X6G10X6B10X6A10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, IDENTITY };
		case ColorFormat::B10X6G10X6R10X6A10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2BGRA };
		case ColorFormat::G10X6R10X6B10X6A10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2GRBA };
		case ColorFormat::G10X6B10X6R10X6A10X6_16:	return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2GBRA };

		case ColorFormat::X2R10G10B10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XRGB };
		case ColorFormat::X2B10G10R10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XBGR };
		case ColorFormat::X2G10R10B10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XGRB };
		case ColorFormat::X2G10B10R10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XGBR };

		case ColorFormat::A2R10G10B10_32:	return { vk::Format::eA2R10G10B10UnormPack32, IDENTITY };
		case ColorFormat::A2B10G10R10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2ABGR };
		case ColorFormat::A2G10R10B10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2AGRB };
		case ColorFormat::A2G10B10R10_32:	return { vk::Format::eA2R10G10B10UnormPack32, ARGB2AGBR };

		case ColorFormat::R10X6G10X6B10X6G10X6_S422_16:	return { vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16, RGBA2BGRA };
		case ColorFormat::B10X6G10X6R10X6G10X6_S422_16:	return { vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16, IDENTITY };
		case ColorFormat::G10X6R10X6G10X6B10X6_S422_16:	return { vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16, RGBA2BGRA };
		case ColorFormat::G10X6B10X6G10X6R10X6_S422_16:	return { vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16, IDENTITY };

		case ColorFormat::G10X6_B10X6_R10X6_16:			return { vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16, IDENTITY };
		case ColorFormat::G10X6_B10X6_R10X6_S422_16:	return { vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16, IDENTITY };
		case ColorFormat::G10X6_B10X6_R10X6_S420_16:	return { vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16, IDENTITY };

		case ColorFormat::G10X6_R10X6B10X6_S422_16:		return { vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16, RGBA2BGRA };
		case ColorFormat::G10X6_B10X6R10X6_S422_16:		return { vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16, IDENTITY };
		case ColorFormat::G10X6_R10X6B10X6_S420_16:		return { vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16, RGBA2BGRA };
		case ColorFormat::G10X6_B10X6R10X6_S420_16:		return { vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16, IDENTITY };

		//12 bits per component
		case ColorFormat::Y12X4_16:			return { vk::Format::eR12X4UnormPack16, R2Y };

		case ColorFormat::Y12X4A12X4_16:	return { vk::Format::eR12X4G12X4Unorm2Pack16, RG2YA };
		case ColorFormat::A12X4Y12X4_16:	return { vk::Format::eR12X4G12X4Unorm2Pack16, RG2AY };

		case ColorFormat::A12X4R12X4G12X4B12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2ARGB };
		case ColorFormat::A12X4B12X4G12X4R12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2ABGR };
		case ColorFormat::A12X4G12X4R12X4B12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2AGRB };
		case ColorFormat::A12X4G12X4B12X4R12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2AGBR };
		case ColorFormat::R12X4G12X4B12X4A12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, IDENTITY };
		case ColorFormat::B12X4G12X4R12X4A12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2BGRA };
		case ColorFormat::G12X4R12X4B12X4A12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2GRBA };
		case ColorFormat::G12X4B12X4R12X4A12X4_16:	return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2GBRA };

		case ColorFormat::R12X4G12X4B12X4G12X4_S422_16:	return { vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16, RGBA2BGRA };
		case ColorFormat::B12X4G12X4R12X4G12X4_S422_16:	return { vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16, IDENTITY };
		case ColorFormat::G12X4R12X4G12X4B12X4_S422_16:	return { vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16, RGBA2BGRA };
		case ColorFormat::G12X4B12X4G12X4R12X4_S422_16:	return { vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16, IDENTITY };

		case ColorFormat::G12X4_B12X4_R12X4_16:			return { vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16, IDENTITY };
		case ColorFormat::G12X4_B12X4_R12X4_S422_16:	return { vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16, IDENTITY };
		case ColorFormat::G12X4_B12X4_R12X4_S420_16:	return { vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16, IDENTITY };

		case ColorFormat::G12X4_R12X4B12X4_S422_16:		return { vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16, RGBA2BGRA };
		case ColorFormat::G12X4_B12X4R12X4_S422_16:		return { vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16, IDENTITY };
		case ColorFormat::G12X4_R12X4B12X4_S420_16:		return { vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16, RGBA2BGRA };
		case ColorFormat::G12X4_B12X4R12X4_S420_16:		return { vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16, IDENTITY };


		//16 bits per component
		case ColorFormat::Y16:				return { vk::Format::eR16Unorm, R2Y };

		case ColorFormat::Y16A16:			return { vk::Format::eR16G16Unorm, RG2YA };
		case ColorFormat::A16Y16:			return { vk::Format::eR16G16Unorm, RG2AY };
			
		case ColorFormat::R16G16B16:		return { vk::Format::eR16G16B16Unorm, IDENTITY };
		case ColorFormat::B16G16R16:		return { vk::Format::eR16G16B16Unorm, RGBA2BGRA };
		case ColorFormat::G16R16B16:		return { vk::Format::eR16G16B16Unorm, RGBA2GRBA };
		case ColorFormat::G16B16R16:		return { vk::Format::eR16G16B16Unorm, RGBA2GBRA };

		case ColorFormat::A16R16G16B16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2ARGB };
		case ColorFormat::A16B16G16R16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2ABGR };
		case ColorFormat::A16G16R16B16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2AGRB };
		case ColorFormat::A16G16B16R16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2AGBR };
		case ColorFormat::R16G16B16A16:		return { vk::Format::eR16G16B16A16Unorm, IDENTITY };
		case ColorFormat::B16G16R16A16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2BGRA };
		case ColorFormat::G16R16B16A16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2GRBA };
		case ColorFormat::G16B16R16A16:		return { vk::Format::eR16G16B16A16Unorm, RGBA2GBRA };

		case ColorFormat::R16G16B16G16_S422:	return { vk::Format::eB16G16R16G16422Unorm, RGBA2BGRA };
		case ColorFormat::B16G16R16G16_S422:	return { vk::Format::eB16G16R16G16422Unorm, IDENTITY };
		case ColorFormat::G16R16G16B16_S422:	return { vk::Format::eG16B16G16R16422Unorm, RGBA2BGRA };
		case ColorFormat::G16B16G16R16_S422:	return { vk::Format::eG16B16G16R16422Unorm, IDENTITY };

		case ColorFormat::G16_B16_R16:		return { vk::Format::eG16B16R163Plane444Unorm, IDENTITY };
		case ColorFormat::G16_B16_R16_S422:	return { vk::Format::eG16B16R163Plane422Unorm, IDENTITY };
		case ColorFormat::G16_B16_R16_S420:	return { vk::Format::eG16B16R163Plane420Unorm, IDENTITY };

		case ColorFormat::G16_R16B16_S422:	return { vk::Format::eG16B16R162Plane422Unorm, RGBA2BGRA };
		case ColorFormat::G16_B16R16_S422:	return { vk::Format::eG16B16R162Plane422Unorm, IDENTITY };
		case ColorFormat::G16_R16B16_S420:	return { vk::Format::eG16B16R162Plane420Unorm, RGBA2BGRA };
		case ColorFormat::G16_B16R16_S420:	return { vk::Format::eG16B16R162Plane420Unorm, IDENTITY };

		default: return {};
		}

	case ColorEncoding::INT:
		switch(fmt){
		//8 bit components
		case ColorFormat::Y8:				return { vk::Format::eR8Snorm, R2Y };

		case ColorFormat::Y8A8:				return { vk::Format::eR8G8Snorm, RG2YA };
		case ColorFormat::A8Y8:				return { vk::Format::eR8G8Snorm, RG2AY };

		case ColorFormat::R8G8B8:			return { vk::Format::eR8G8B8Snorm, IDENTITY };
		case ColorFormat::B8G8R8:			return { vk::Format::eR8G8B8Snorm, RGBA2BGRA };
		case ColorFormat::G8R8B8:			return { vk::Format::eR8G8B8Snorm, RGBA2GRBA };
		case ColorFormat::G8B8R8:			return { vk::Format::eR8G8B8Snorm, RGBA2GBRA };

		case ColorFormat::A8R8G8B8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2ARGB };
		case ColorFormat::A8B8G8R8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2ABGR };
		case ColorFormat::A8G8R8B8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2AGRB };
		case ColorFormat::A8G8B8R8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2AGBR };
		case ColorFormat::R8G8B8A8:			return { vk::Format::eR8G8B8A8Snorm, IDENTITY };
		case ColorFormat::B8G8R8A8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2BGRA };
		case ColorFormat::G8R8B8A8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2GRBA };
		case ColorFormat::G8B8R8A8:			return { vk::Format::eR8G8B8A8Snorm, RGBA2GBRA };

		#if ZUAZO_IS_BIG_ENDIAN
		case ColorFormat::X8R8G8B8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2XRGB };
		case ColorFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2XBGR };
		case ColorFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2RGBX };
		case ColorFormat::B8G8R8X8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2BGRX };

		case ColorFormat::A8R8G8B8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2ARGB };
		case ColorFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2ABGR };
		case ColorFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Snorm, IDENTITY };
		case ColorFormat::B8G8R8A8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2BGRA };
		#elif ZUAZO_IS_LITTLE_ENDIAN
		case ColorFormat::X8R8G8B8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2BGRX };
		case ColorFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2RGBX };
		case ColorFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2XBGR };
		case ColorFormat::B8G8R8X8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2XRGB };

		case ColorFormat::A8R8G8B8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2BGRA };
		case ColorFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Snorm, IDENTITY };
		case ColorFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2ABGR };
		case ColorFormat::B8G8R8A8_32:		return { vk::Format::eR8G8B8A8Snorm, RGBA2ARGB };
		#endif

		//16 bits per component
		case ColorFormat::Y16:				return { vk::Format::eR16Snorm, R2Y };

		case ColorFormat::Y16A16:			return { vk::Format::eR16G16Snorm, RG2YA };
		case ColorFormat::A16Y16:			return { vk::Format::eR16G16Snorm, RG2AY };
			
		case ColorFormat::R16G16B16:		return { vk::Format::eR16G16B16Snorm, IDENTITY };
		case ColorFormat::B16G16R16:		return { vk::Format::eR16G16B16Snorm, RGBA2BGRA };
		case ColorFormat::G16R16B16:		return { vk::Format::eR16G16B16Snorm, RGBA2GRBA };
		case ColorFormat::G16B16R16:		return { vk::Format::eR16G16B16Snorm, RGBA2GBRA };

		case ColorFormat::A16R16G16B16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2ARGB };
		case ColorFormat::A16B16G16R16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2ABGR };
		case ColorFormat::A16G16R16B16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2AGRB };
		case ColorFormat::A16G16B16R16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2AGBR };
		case ColorFormat::R16G16B16A16:		return { vk::Format::eR16G16B16A16Snorm, IDENTITY };
		case ColorFormat::B16G16R16A16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2BGRA };
		case ColorFormat::G16R16B16A16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2GRBA };
		case ColorFormat::G16B16R16A16:		return { vk::Format::eR16G16B16A16Snorm, RGBA2GBRA };

		default: return {};
		}
	case ColorEncoding::IEEE754:
		switch(fmt){
		//16 bits per component
		case ColorFormat::Y16:				return { vk::Format::eR16Sfloat, R2Y };

		case ColorFormat::Y16A16:			return { vk::Format::eR16G16Sfloat, RG2YA };
		case ColorFormat::A16Y16:			return { vk::Format::eR16G16Sfloat, RG2AY };
			
		case ColorFormat::R16G16B16:		return { vk::Format::eR16G16B16Sfloat, IDENTITY };
		case ColorFormat::B16G16R16:		return { vk::Format::eR16G16B16Sfloat, RGBA2BGRA };
		case ColorFormat::G16R16B16:		return { vk::Format::eR16G16B16Sfloat, RGBA2GRBA };
		case ColorFormat::G16B16R16:		return { vk::Format::eR16G16B16Sfloat, RGBA2GBRA };

		case ColorFormat::A16R16G16B16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2ARGB };
		case ColorFormat::A16B16G16R16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2ABGR };
		case ColorFormat::A16G16R16B16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2AGRB };
		case ColorFormat::A16G16B16R16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2AGBR };
		case ColorFormat::R16G16B16A16:		return { vk::Format::eR16G16B16A16Sfloat, IDENTITY };
		case ColorFormat::B16G16R16A16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2BGRA };
		case ColorFormat::G16R16B16A16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2GRBA };
		case ColorFormat::G16B16R16A16:		return { vk::Format::eR16G16B16A16Sfloat, RGBA2GBRA };

		//32 bits per component
		case ColorFormat::Y32:				return { vk::Format::eR32Sfloat, R2Y };

		case ColorFormat::Y32A32:			return { vk::Format::eR32G32Sfloat, RG2YA };
		case ColorFormat::A32Y32:			return { vk::Format::eR32G32Sfloat, RG2AY };
			
		case ColorFormat::R32G32B32:		return { vk::Format::eR32G32B32Sfloat, IDENTITY };
		case ColorFormat::B32G32R32:		return { vk::Format::eR32G32B32Sfloat, RGBA2BGRA };
		case ColorFormat::G32R32B32:		return { vk::Format::eR32G32B32Sfloat, RGBA2GRBA };
		case ColorFormat::G32B32R32:		return { vk::Format::eR32G32B32Sfloat, RGBA2GBRA };

		case ColorFormat::A32R32G32B32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2ARGB };
		case ColorFormat::A32B32G32R32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2ABGR };
		case ColorFormat::A32G32R32B32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2AGRB };
		case ColorFormat::A32G32B32R32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2AGBR };
		case ColorFormat::R32G32B32A32:		return { vk::Format::eR32G32B32A32Sfloat, IDENTITY };
		case ColorFormat::B32G32R32A32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2BGRA };
		case ColorFormat::G32R32B32A32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2GRBA };
		case ColorFormat::G32B32R32A32:		return { vk::Format::eR32G32B32A32Sfloat, RGBA2GBRA };

		//64 bits per component
		case ColorFormat::Y64:				return { vk::Format::eR64Sfloat, R2Y };

		case ColorFormat::Y64A64:			return { vk::Format::eR64G64Sfloat, RG2YA };
		case ColorFormat::A64Y64:			return { vk::Format::eR64G64Sfloat, RG2AY };
			
		case ColorFormat::R64G64B64:		return { vk::Format::eR64G64B64Sfloat, IDENTITY };
		case ColorFormat::B64G64R64:		return { vk::Format::eR64G64B64Sfloat, RGBA2BGRA };
		case ColorFormat::G64R64B64:		return { vk::Format::eR64G64B64Sfloat, RGBA2GRBA };
		case ColorFormat::G64B64R64:		return { vk::Format::eR64G64B64Sfloat, RGBA2GBRA };

		case ColorFormat::A64R64G64B64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2ARGB };
		case ColorFormat::A64B64G64R64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2ABGR };
		case ColorFormat::A64G64R64B64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2AGRB };
		case ColorFormat::A64G64B64R64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2AGBR };
		case ColorFormat::R64G64B64A64:		return { vk::Format::eR64G64B64A64Sfloat, IDENTITY };
		case ColorFormat::B64G64R64A64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2BGRA };
		case ColorFormat::G64R64B64A64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2GRBA };
		case ColorFormat::G64B64R64A64:		return { vk::Format::eR64G64B64A64Sfloat, RGBA2GBRA };

		default: return {};
		}
	case ColorEncoding::IEC61966_2_1:
		switch(fmt){
		//8 bit components
		case ColorFormat::Y8:				return { vk::Format::eR8Srgb, R2Y };

		case ColorFormat::Y8A8:				return { vk::Format::eR8G8Srgb, RG2YA };
		case ColorFormat::A8Y8:				return { vk::Format::eR8G8Srgb, RG2AY };

		case ColorFormat::R8G8B8:			return { vk::Format::eR8G8B8Srgb, IDENTITY };
		case ColorFormat::B8G8R8:			return { vk::Format::eR8G8B8Srgb, RGBA2BGRA };
		case ColorFormat::G8R8B8:			return { vk::Format::eR8G8B8Srgb, RGBA2GRBA };
		case ColorFormat::G8B8R8:			return { vk::Format::eR8G8B8Srgb, RGBA2GBRA };

		case ColorFormat::A8R8G8B8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2ARGB };
		case ColorFormat::A8B8G8R8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2ABGR };
		case ColorFormat::A8G8R8B8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2AGRB };
		case ColorFormat::A8G8B8R8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2AGBR };
		case ColorFormat::R8G8B8A8:			return { vk::Format::eR8G8B8A8Srgb, IDENTITY };
		case ColorFormat::B8G8R8A8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2BGRA };
		case ColorFormat::G8R8B8A8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2GRBA };
		case ColorFormat::G8B8R8A8:			return { vk::Format::eR8G8B8A8Srgb, RGBA2GBRA };

		#if ZUAZO_IS_BIG_ENDIAN
		case ColorFormat::X8R8G8B8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2XRGB };
		case ColorFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2XBGR };
		case ColorFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2RGBX };
		case ColorFormat::B8G8R8X8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2BGRX };

		case ColorFormat::A8R8G8B8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2ARGB };
		case ColorFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2ABGR };
		case ColorFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Srgb, IDENTITY };
		case ColorFormat::B8G8R8A8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2BGRA };
		#elif ZUAZO_IS_LITTLE_ENDIAN
		case ColorFormat::X8R8G8B8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2BGRX };
		case ColorFormat::X8B8G8R8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2RGBX };
		case ColorFormat::R8G8B8X8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2XBGR };
		case ColorFormat::B8G8R8X8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2XRGB };

		case ColorFormat::A8R8G8B8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2BGRA };
		case ColorFormat::A8B8G8R8_32:		return { vk::Format::eR8G8B8A8Srgb, IDENTITY };
		case ColorFormat::R8G8B8A8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2ABGR };
		case ColorFormat::B8G8R8A8_32:		return { vk::Format::eR8G8B8A8Srgb, RGBA2ARGB };
		#endif

		default: return {};
		}
	default: return {};
	}
}

constexpr std::tuple<vk::Format, vk::ComponentMapping> removeSwizzle(const std::tuple<vk::Format, vk::ComponentMapping>& fmt){
	const auto format = std::get<vk::Format>(fmt);
	const auto& swizzle = std::get<vk::ComponentMapping>(fmt);

	const auto identitySwizzle = vk::ComponentMapping(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		(swizzle.a == vk::ComponentSwizzle::eA) ? vk::ComponentSwizzle::eIdentity : swizzle.a
	);

	//Check if it is identity already
	if(	(swizzle.r == vk::ComponentSwizzle::eIdentity || swizzle.r == vk::ComponentSwizzle::eR) &&
		(swizzle.g == vk::ComponentSwizzle::eIdentity || swizzle.g == vk::ComponentSwizzle::eG) &&
		(swizzle.b == vk::ComponentSwizzle::eIdentity || swizzle.b == vk::ComponentSwizzle::eB) )
	{
		return { format, identitySwizzle };
	}
	
	//Check if it has reversed color
	if(	swizzle.r == vk::ComponentSwizzle::eB &&
		(swizzle.g == vk::ComponentSwizzle::eIdentity || swizzle.g == vk::ComponentSwizzle::eG) &&
		swizzle.b == vk::ComponentSwizzle::eR )
	{
		switch(format){
		case vk::Format::eR4G4B4A4UnormPack16: 	return { vk::Format::eB4G4R4A4UnormPack16, identitySwizzle };
		case vk::Format::eB4G4R4A4UnormPack16: 	return { vk::Format::eR4G4B4A4UnormPack16, identitySwizzle };

		case vk::Format::eR5G6B5UnormPack16: 	return { vk::Format::eB5G6R5UnormPack16, identitySwizzle };
		case vk::Format::eB5G6R5UnormPack16: 	return { vk::Format::eR5G6B5UnormPack16, identitySwizzle };

		case vk::Format::eR5G5B5A1UnormPack16: 	return { vk::Format::eB5G5R5A1UnormPack16, identitySwizzle };
		case vk::Format::eB5G5R5A1UnormPack16: 	return { vk::Format::eR5G5B5A1UnormPack16, identitySwizzle };

		case vk::Format::eR8G8B8Unorm: 			return { vk::Format::eB8G8R8Unorm, identitySwizzle }; 
		case vk::Format::eB8G8R8Unorm: 			return { vk::Format::eR8G8B8Unorm, identitySwizzle };
		case vk::Format::eR8G8B8Snorm: 			return { vk::Format::eB8G8R8Snorm, identitySwizzle }; 
		case vk::Format::eB8G8R8Snorm: 			return { vk::Format::eR8G8B8Snorm, identitySwizzle };
		case vk::Format::eR8G8B8Uscaled:		return { vk::Format::eB8G8R8Uscaled, identitySwizzle }; 
		case vk::Format::eB8G8R8Uscaled:		return { vk::Format::eR8G8B8Uscaled, identitySwizzle };
		case vk::Format::eR8G8B8Sscaled:		return { vk::Format::eB8G8R8Sscaled, identitySwizzle }; 
		case vk::Format::eB8G8R8Sscaled:		return { vk::Format::eR8G8B8Sscaled, identitySwizzle }; 
		case vk::Format::eR8G8B8Uint:			return { vk::Format::eB8G8R8Uint, identitySwizzle }; 
		case vk::Format::eB8G8R8Uint:			return { vk::Format::eR8G8B8Uint, identitySwizzle }; 
		case vk::Format::eR8G8B8Sint:			return { vk::Format::eB8G8R8Sint, identitySwizzle }; 
		case vk::Format::eB8G8R8Sint:			return { vk::Format::eR8G8B8Sint, identitySwizzle }; 
		case vk::Format::eR8G8B8Srgb:			return { vk::Format::eB8G8R8Srgb, identitySwizzle }; 
		case vk::Format::eB8G8R8Srgb:			return { vk::Format::eR8G8B8Srgb, identitySwizzle }; 

		case vk::Format::eR8G8B8A8Unorm: 		return { vk::Format::eB8G8R8A8Unorm, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Unorm: 		return { vk::Format::eR8G8B8A8Unorm, identitySwizzle };
		case vk::Format::eR8G8B8A8Snorm: 		return { vk::Format::eB8G8R8A8Snorm, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Snorm: 		return { vk::Format::eR8G8B8A8Snorm, identitySwizzle };
		case vk::Format::eR8G8B8A8Uscaled:		return { vk::Format::eB8G8R8A8Uscaled, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Uscaled:		return { vk::Format::eR8G8B8A8Uscaled, identitySwizzle };
		case vk::Format::eR8G8B8A8Sscaled:		return { vk::Format::eB8G8R8A8Sscaled, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Sscaled:		return { vk::Format::eR8G8B8A8Sscaled, identitySwizzle }; 
		case vk::Format::eR8G8B8A8Uint:			return { vk::Format::eB8G8R8A8Uint, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Uint:			return { vk::Format::eR8G8B8A8Uint, identitySwizzle }; 
		case vk::Format::eR8G8B8A8Sint:			return { vk::Format::eB8G8R8A8Sint, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Sint:			return { vk::Format::eR8G8B8A8Sint, identitySwizzle }; 
		case vk::Format::eR8G8B8A8Srgb:			return { vk::Format::eB8G8R8A8Srgb, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Srgb:			return { vk::Format::eR8G8B8A8Srgb, identitySwizzle }; 

		case vk::Format::eA2R10G10B10UnormPack32:	return  { vk::Format::eA2B10G10R10UnormPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10UnormPack32:	return  { vk::Format::eA2R10G10B10UnormPack32, identitySwizzle };
		case vk::Format::eA2R10G10B10SnormPack32:	return  { vk::Format::eA2B10G10R10SnormPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10SnormPack32:	return  { vk::Format::eA2R10G10B10SnormPack32, identitySwizzle };
		case vk::Format::eA2R10G10B10UscaledPack32:	return  { vk::Format::eA2B10G10R10UscaledPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10UscaledPack32:	return  { vk::Format::eA2R10G10B10UscaledPack32, identitySwizzle };
		case vk::Format::eA2R10G10B10SscaledPack32:	return  { vk::Format::eA2B10G10R10SscaledPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10SscaledPack32:	return  { vk::Format::eA2R10G10B10SscaledPack32, identitySwizzle };
		case vk::Format::eA2R10G10B10UintPack32:	return  { vk::Format::eA2B10G10R10UintPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10UintPack32:	return  { vk::Format::eA2R10G10B10UintPack32, identitySwizzle };
		case vk::Format::eA2R10G10B10SintPack32:	return  { vk::Format::eA2B10G10R10SintPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10SintPack32:	return  { vk::Format::eA2R10G10B10SintPack32, identitySwizzle };

		default: break;
		}
	}

	return fmt; //No conversion was found
}

constexpr std::tuple<ColorFormat, ColorEncoding> fromVulkan(vk::Format fmt){
	switch(fmt){
	//4 bit formats
	case vk::Format::eR4G4B4A4UnormPack16: 	return { ColorFormat::R4G4B4A4_16, ColorEncoding::UINT };
	case vk::Format::eB4G4R4A4UnormPack16: 	return { ColorFormat::B4G4R4A4_16, ColorEncoding::UINT };

	//5 bits formats
	case vk::Format::eR5G5B5A1UnormPack16: 	return { ColorFormat::R5G5B5A1_16, ColorEncoding::UINT };
	case vk::Format::eB5G5R5A1UnormPack16: 	return { ColorFormat::B5G5R5A1_16, ColorEncoding::UINT };
	case vk::Format::eA1R5G5B5UnormPack16: 	return { ColorFormat::A1R5G5B5_16, ColorEncoding::UINT };

	//6 bit formats
	case vk::Format::eR5G6B5UnormPack16:	return { ColorFormat::R5G6B5_16, ColorEncoding::UINT };
	case vk::Format::eB5G6R5UnormPack16:	return { ColorFormat::B5G6R5_16, ColorEncoding::UINT };

	//8bit formats
	case vk::Format::eR8G8B8Unorm:			return { ColorFormat::R8G8B8, ColorEncoding::UINT };
	case vk::Format::eR8G8B8Snorm:			return { ColorFormat::R8G8B8, ColorEncoding::INT };
	case vk::Format::eR8G8B8Srgb:			return { ColorFormat::R8G8B8, ColorEncoding::IEC61966_2_1 };
	case vk::Format::eB8G8R8Unorm:			return { ColorFormat::B8G8R8, ColorEncoding::UINT };
	case vk::Format::eB8G8R8Snorm:			return { ColorFormat::B8G8R8, ColorEncoding::INT };
	case vk::Format::eB8G8R8Srgb:			return { ColorFormat::B8G8R8, ColorEncoding::IEC61966_2_1 };
	case vk::Format::eR8G8B8A8Unorm:		return { ColorFormat::R8G8B8A8, ColorEncoding::UINT };
	case vk::Format::eR8G8B8A8Snorm:		return { ColorFormat::R8G8B8A8, ColorEncoding::INT };
	case vk::Format::eR8G8B8A8Srgb:			return { ColorFormat::R8G8B8A8, ColorEncoding::IEC61966_2_1 };
	case vk::Format::eB8G8R8A8Unorm:		return { ColorFormat::B8G8R8A8, ColorEncoding::UINT };
	case vk::Format::eB8G8R8A8Snorm:		return { ColorFormat::B8G8R8A8, ColorEncoding::INT };
	case vk::Format::eB8G8R8A8Srgb:			return { ColorFormat::B8G8R8A8, ColorEncoding::IEC61966_2_1 };
	case vk::Format::eG8B8G8R8422Unorm:		return { ColorFormat::G8B8G8R8_S422, ColorEncoding::UINT };
	case vk::Format::eB8G8R8G8422Unorm:		return { ColorFormat::B8G8R8G8_S422, ColorEncoding::UINT };
	case vk::Format::eG8B8R83Plane420Unorm:	return { ColorFormat::G8_B8_R8_S420, ColorEncoding::UINT };
	case vk::Format::eG8B8R83Plane422Unorm:	return { ColorFormat::G8_B8_R8_S422, ColorEncoding::UINT };
	case vk::Format::eG8B8R83Plane444Unorm:	return { ColorFormat::G8_B8_R8, ColorEncoding::UINT };
	case vk::Format::eG8B8R82Plane420Unorm:	return { ColorFormat::G8_B8R8_S420, ColorEncoding::UINT };
	case vk::Format::eG8B8R82Plane422Unorm:	return { ColorFormat::G8_B8R8_S422, ColorEncoding::UINT };

	//10 bit formats
	case vk::Format::eA2R10G10B10UnormPack32:	return { ColorFormat::A2R10G10B10_32, ColorEncoding::UINT };
	case vk::Format::eA2R10G10B10SnormPack32:	return { ColorFormat::A2R10G10B10_32, ColorEncoding::INT };
	case vk::Format::eA2B10G10R10UnormPack32:	return { ColorFormat::A2B10G10R10_32, ColorEncoding::UINT };
	case vk::Format::eA2B10G10R10SnormPack32:	return { ColorFormat::A2B10G10R10_32, ColorEncoding::INT };
	case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16:		return { ColorFormat::R10X6G10X6B10X6A10X6_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16:	return { ColorFormat::G10X6B10X6G10X6R10X6_S422_16, ColorEncoding::UINT };
	case vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16:	return { ColorFormat::B10X6G10X6R10X6G10X6_S422_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16:	return { ColorFormat::G10X6_B10X6_R10X6_S420_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16:	return { ColorFormat::G10X6_B10X6_R10X6_S422_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16:	return { ColorFormat::G10X6_B10X6_R10X6_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16:	return { ColorFormat::G10X6_B10X6R10X6_S420_16, ColorEncoding::UINT };
	case vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16:	return { ColorFormat::G10X6_B10X6R10X6_S422_16, ColorEncoding::UINT };

	//12 bit formats
	case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16:		return { ColorFormat::R12X4G12X4B12X4A12X4_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16:	return { ColorFormat::G12X4B12X4G12X4R12X4_S422_16, ColorEncoding::UINT };
	case vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16:	return { ColorFormat::B12X4G12X4R12X4G12X4_S422_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16:	return { ColorFormat::G12X4_B12X4_R12X4_S420_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16:	return { ColorFormat::G12X4_B12X4_R12X4_S422_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16:	return { ColorFormat::G12X4_B12X4_R12X4_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16:	return { ColorFormat::G12X4_B12X4R12X4_S420_16, ColorEncoding::UINT };
	case vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16:	return { ColorFormat::G12X4_B12X4R12X4_S422_16, ColorEncoding::UINT };

	//16 bit formats
	case vk::Format::eR16G16B16Unorm:			return { ColorFormat::R16G16B16, ColorEncoding::UINT };
	case vk::Format::eR16G16B16Snorm:			return { ColorFormat::R16G16B16, ColorEncoding::INT };
	case vk::Format::eR16G16B16Sfloat:			return { ColorFormat::R16G16B16, ColorEncoding::IEEE754 };
	case vk::Format::eR16G16B16A16Unorm:		return { ColorFormat::R16G16B16A16, ColorEncoding::UINT };
	case vk::Format::eR16G16B16A16Snorm:		return { ColorFormat::R16G16B16A16, ColorEncoding::INT };
	case vk::Format::eR16G16B16A16Sfloat:		return { ColorFormat::R16G16B16A16, ColorEncoding::IEEE754 };
	case vk::Format::eG16B16G16R16422Unorm:		return { ColorFormat::G16B16G16R16_S422, ColorEncoding::UINT };
	case vk::Format::eB16G16R16G16422Unorm:		return { ColorFormat::B16G16R16G16_S422, ColorEncoding::UINT };
	case vk::Format::eG16B16R163Plane420Unorm:	return { ColorFormat::G16_B16_R16_S420, ColorEncoding::UINT };
	case vk::Format::eG16B16R163Plane422Unorm:	return { ColorFormat::G16_B16_R16_S422, ColorEncoding::UINT };
	case vk::Format::eG16B16R163Plane444Unorm:	return { ColorFormat::G16_B16_R16, ColorEncoding::UINT };
	case vk::Format::eG16B16R162Plane420Unorm:	return { ColorFormat::G16_B16R16_S420, ColorEncoding::UINT };
	case vk::Format::eG16B16R162Plane422Unorm:	return { ColorFormat::G16_B16R16_S422, ColorEncoding::UINT };

	//32 bit formats
	case vk::Format::eR32G32B32Sfloat:		return { ColorFormat::R32G32B32, ColorEncoding::IEEE754 };
	case vk::Format::eR32G32B32A32Sfloat:	return { ColorFormat::R32G32B32A32, ColorEncoding::IEEE754 };

	//64 bit formats
	case vk::Format::eR64G64B64Sfloat:		return { ColorFormat::R64G64B64, ColorEncoding::IEEE754 };
	case vk::Format::eR64G64B64A64Sfloat:	return { ColorFormat::R64G64B64A64, ColorEncoding::IEEE754 };

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

inline vk::SurfaceFormatKHR toVulkan(ColorFormat fmt, ColorPrimaries prim, ColorEncoding enc){
	const auto [format, swizzle] = removeSwizzle(toVulkan(fmt, enc));
	if(format == vk::Format::eUndefined || swizzle != vk::ComponentMapping()) return {};

	const vk::ColorSpaceKHR space = toVulkan(prim, enc);
	if(space == static_cast<vk::ColorSpaceKHR>(-1)) return {};
	
	return vk::SurfaceFormatKHR(
		format,
		space		
	);
}

constexpr std::tuple<ColorFormat, ColorPrimaries, ColorEncoding> fromVulkan(const vk::SurfaceFormatKHR& fmt){
	const auto[format, enc] = fromVulkan(fmt.format);
	if(format == ColorFormat::NONE || enc == ColorEncoding::NONE) return {};

	const auto[prim, enc2] = fromVulkan(fmt.colorSpace);
	if(prim == ColorPrimaries::NONE) return {};
	if(enc2 == ColorEncoding::NONE){
		//Linear encoding
		switch(enc){
		case ColorEncoding::UINT:
		case ColorEncoding::UINT_ITU_NARROW:
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
