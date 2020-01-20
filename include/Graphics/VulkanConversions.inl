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

constexpr std::tuple<vk::Format, vk::ComponentMapping> toVulkan(PixelFormat fmt, ColorEncoding enc){
	constexpr vk::ComponentMapping IDENTITY;

	constexpr vk::ComponentMapping R2Y(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eOne
	);

	constexpr vk::ComponentMapping RG2YX(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping RG2XY(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping RG2YA(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eG
	);
	constexpr vk::ComponentMapping RG2AY(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR
	);

	constexpr vk::ComponentMapping ARGB2XRGB(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ARGB2XBGR(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ARGB2XGRB(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ARGB2XGBR(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ARGB2RGBX(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ARGB2BGRX(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ARGB2GRBX(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ARGB2GBRX(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ARGB2ARGB = IDENTITY;
	constexpr vk::ComponentMapping ARGB2ABGR(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eIdentity
	);
	constexpr vk::ComponentMapping ARGB2AGRB(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity
	);
	constexpr vk::ComponentMapping ARGB2AGBR(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eIdentity
	);
	constexpr vk::ComponentMapping ARGB2RGBA(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eB
	);
	constexpr vk::ComponentMapping ARGB2BGRA(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eB
	);
	constexpr vk::ComponentMapping ARGB2GRBA(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eB
	);
	constexpr vk::ComponentMapping ARGB2GBRA(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eB
	);
	
	constexpr vk::ComponentMapping ABGR2XRGB(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ABGR2XBGR(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ABGR2XGRB(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ABGR2XGBR(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ABGR2RGBX(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ABGR2BGRX(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ABGR2GRBX(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ABGR2GBRX(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping ABGR2ARGB(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eIdentity
	);
	constexpr vk::ComponentMapping ABGR2ABGR = IDENTITY;
	constexpr vk::ComponentMapping ABGR2AGRB(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eIdentity
	);
	constexpr vk::ComponentMapping ABGR2AGBR(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eIdentity
	);
	constexpr vk::ComponentMapping ABGR2RGBA(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR
	);
	constexpr vk::ComponentMapping ABGR2BGRA(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eR
	);
	constexpr vk::ComponentMapping ABGR2GRBA(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR
	);
	constexpr vk::ComponentMapping ABGR2GBRA(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR
	);

	constexpr vk::ComponentMapping RGBA2XRGB(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping RGBA2XBGR(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping RGBA2XGRB(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping RGBA2XGBR(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping RGBA2RGBX(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping RGBA2BGRX(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping RGBA2GRBX(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping RGBA2GBRX(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping RGBA2ARGB(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eR
	);
	constexpr vk::ComponentMapping RGBA2ABGR(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR
	);
	constexpr vk::ComponentMapping RGBA2AGRB(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eR
	);
	constexpr vk::ComponentMapping RGBA2AGBR(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR
	);
	constexpr vk::ComponentMapping RGBA2RGBA = IDENTITY;
	constexpr vk::ComponentMapping RGBA2BGRA(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eIdentity
	);
	constexpr vk::ComponentMapping RGBA2GRBA(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity
	);
	constexpr vk::ComponentMapping RGBA2GBRA(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eIdentity
	);

	constexpr vk::ComponentMapping BGRA2XRGB(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping BGRA2XBGR(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping BGRA2XGRB(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping BGRA2XGBR(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping BGRA2RGBX(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping BGRA2BGRX(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping BGRA2GRBX(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping BGRA2GBRX(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eOne
	);
	constexpr vk::ComponentMapping BGRA2ARGB(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eB
	);
	constexpr vk::ComponentMapping BGRA2ABGR(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eB
	);
	constexpr vk::ComponentMapping BGRA2AGRB(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eR
	);
	constexpr vk::ComponentMapping BGRA2AGBR(
		vk::ComponentSwizzle::eA,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eB
	);
	constexpr vk::ComponentMapping BGRA2RGBA(
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eIdentity
	);
	constexpr vk::ComponentMapping BGRA2BGRA = IDENTITY;
	constexpr vk::ComponentMapping BGRA2GRBA(
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eR,
		vk::ComponentSwizzle::eIdentity
	);
	constexpr vk::ComponentMapping BGRA2GBRA(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eB,
		vk::ComponentSwizzle::eG,
		vk::ComponentSwizzle::eIdentity
	);



	switch (fmt){
	//4 bits per compoenent
	case PixelFormat::Y4X4_8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4UnormPack8, RG2YX };
		default:
			return {};
		}
	case PixelFormat::X4Y4_8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4UnormPack8, RG2XY };
		default:
			return {};
		}
	case PixelFormat::Y4A4_8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4UnormPack8, RG2YA };
		default:
			return {};
		}
	case PixelFormat::A4Y4_8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4UnormPack8, RG2AY };
		default:
			return {};
		}

	case PixelFormat::X4R4G4B4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2XRGB };
		default:
			return {};
		}
	case PixelFormat::X4B4G4R4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16,RGBA2XBGR };
		default:
			return {};
		}
	case PixelFormat::X4G4R4B4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2XRGB };
		default:
			return {};
		}
	case PixelFormat::X4G4B4R4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2XGBR };
		default:
			return {};
		}
	case PixelFormat::R4G4B4X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2RGBX };
		default:
			return {};
		}
	case PixelFormat::B4G4R4X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB4G4R4A4UnormPack16, BGRA2BGRX };
		default:
			return {};
		}
	case PixelFormat::G4R4B4X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GRBX };
		default:
			return {};
		}
	case PixelFormat::G4B4R4X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GBRX };
		default:
			return {};
		}

	case PixelFormat::A4R4G4B4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2ARGB };
		default:
			return {};
		}
	case PixelFormat::A4B4G4R4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2ABGR };
		default:
			return {};
		}
	case PixelFormat::A4G4R4B4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2ARGB };
		default:
			return {};
		}
	case PixelFormat::A4G4B4R4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2AGBR };
		default:
			return {};
		}
	case PixelFormat::R4G4B4A4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B4G4R4A4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB4G4R4A4UnormPack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G4R4B4A4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G4B4R4A4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR4G4B4A4UnormPack16, RGBA2GBRA };
		default:
			return {};
		}


	//5 bits per component
	case PixelFormat::X1R5G5B5_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XRGB };
		default:
			return {};
		}
	case PixelFormat::X1B5G5R5_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XBGR };
		default:
			return {};
	}
	case PixelFormat::X1G5R5B5_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XGRB };
		default:
			return {};
		}
	case PixelFormat::X1G5B5R5_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA1R5G5B5UnormPack16, ARGB2XGBR };
		default:
			return {};
	}
	case PixelFormat::R5G5B5X1_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR5G5B5A1UnormPack16, RGBA2RGBX };
		default:
			return {};
		}
	case PixelFormat::B5G5R5X1_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB5G5R5A1UnormPack16, BGRA2BGRX };
		default:
			return {};
	}
	case PixelFormat::G5R5B5X1_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR5G5B5A1UnormPack16, RGBA2GRBX };
		default:
			return {};
		}
	case PixelFormat::G5B5R5X1_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB5G5R5A1UnormPack16, RGBA2GBRX };
		default:
			return {};
	}

	case PixelFormat::A1R5G5B5_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA1R5G5B5UnormPack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::A1B5G5R5_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA1R5G5B5UnormPack16, ARGB2ABGR };
		default:
			return {};
	}
	case PixelFormat::A1G5R5B5_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA1R5G5B5UnormPack16, ARGB2AGRB };
		default:
			return {};
		}
	case PixelFormat::A1G5B5R5_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA1R5G5B5UnormPack16, ARGB2AGBR };
		default:
			return {};
	}
	case PixelFormat::R5G5B5A1_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR5G5B5A1UnormPack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B5G5R5A1_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB5G5R5A1UnormPack16, IDENTITY };
		default:
			return {};
	}
	case PixelFormat::G5R5B5A1_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR5G5B5A1UnormPack16, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G5B5R5A1_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB5G5R5A1UnormPack16, RGBA2GBRA };
		default:
			return {};
	}


 	//6 bits per component
	case PixelFormat::R5G6B5_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR5G6B5UnormPack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B5G6R5_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB5G6R5UnormPack16, IDENTITY };
		default:
			return {};
	}


	//8 bits per component
	case PixelFormat::Y8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8Unorm, R2Y };
		case ColorEncoding::INT:
			return { vk::Format::eR8Snorm, R2Y };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8Srgb, R2Y };
		default:
			return {};
		}

	case PixelFormat::Y8A8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8Unorm, RG2YA };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8Snorm, RG2YA };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8Srgb, RG2YA };
		default:
			return {};
		}
	case PixelFormat::A8Y8:
	switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8Unorm, RG2AY };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8Snorm, RG2AY };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8Srgb, RG2AY };
		default:
			return {};
	}
		
	case PixelFormat::R8G8B8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8B8Unorm, IDENTITY };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8B8Snorm, IDENTITY };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8B8Srgb, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B8G8R8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB8G8R8Unorm, IDENTITY };
		case ColorEncoding::INT:
			return { vk::Format::eB8G8R8Snorm, IDENTITY };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eB8G8R8Srgb, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G8R8B8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8B8Unorm, RGBA2GRBA };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8B8Snorm, RGBA2GRBA };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8B8Srgb, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G8B8R8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8B8Unorm, RGBA2GBRA };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8B8Snorm, RGBA2GBRA };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8B8Srgb, RGBA2GBRA };
		default:
			return {};
		}

	case PixelFormat::A8R8G8B8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8B8A8Unorm, RGBA2ARGB };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8B8A8Snorm, RGBA2ARGB };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8B8A8Srgb, RGBA2ARGB };
		default:
			return {};
		}
	case PixelFormat::A8B8G8R8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8B8A8Unorm, RGBA2ABGR };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8B8A8Snorm, RGBA2ABGR };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8B8A8Srgb, RGBA2ABGR };
		default:
			return {};
		}
	case PixelFormat::A8G8R8B8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8B8A8Unorm, RGBA2AGRB };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8B8A8Snorm, RGBA2AGRB };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8B8A8Srgb, RGBA2AGRB };
		default:
			return {};
		}
	case PixelFormat::A8G8B8R8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8B8A8Unorm, RGBA2AGBR };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8B8A8Snorm, RGBA2AGBR };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8B8A8Srgb, RGBA2AGBR };
		default:
			return {};
		}
	case PixelFormat::R8G8B8A8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8B8A8Unorm, IDENTITY };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8B8A8Snorm, IDENTITY };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8B8A8Srgb, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B8G8R8A8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB8G8R8Unorm, IDENTITY };
		case ColorEncoding::INT:
			return { vk::Format::eB8G8R8Snorm, IDENTITY };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eB8G8R8Srgb, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G8R8B8A8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8B8A8Unorm, RGBA2GRBA };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8B8A8Snorm, RGBA2GRBA };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8B8A8Srgb, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G8B8R8A8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR8G8B8A8Unorm, RGBA2GBRA };
		case ColorEncoding::INT:
			return { vk::Format::eR8G8B8A8Snorm, RGBA2GBRA };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eR8G8B8A8Srgb, RGBA2GBRA };
		default:
			return {};
		}

	case PixelFormat::X8R8G8B8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2XRGB };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2XRGB };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2XRGB };
		default:
			return {};
		}
	case PixelFormat::X8B8G8R8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2XBGR };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2XBGR };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2XBGR };
		default:
			return {};
		}
	case PixelFormat::X8G8R8B8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2XRGB };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2XRGB };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2XRGB };
		default:
			return {};
		}
	case PixelFormat::X8G8B8R8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2XGBR };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2XGBR };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2XGBR };
		default:
			return {};
		}
	case PixelFormat::R8G8B8X8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2RGBX };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2RGBX };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2RGBX };
		default:
			return {};
		}
	case PixelFormat::B8G8R8X8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2BGRX };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2BGRX };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2BGRX };
		default:
			return {};
		}
	case PixelFormat::G8R8B8X8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2GRBX };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2GRBX };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2GRBX };
		default:
			return {};
		}
	case PixelFormat::G8B8R8X8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2GBRX };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2GBRX };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2GBRX };
		default:
			return {};
		}
	case PixelFormat::A8R8G8B8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2ARGB };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2ARGB };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2ARGB };
		default:
			return {};
		}
	case PixelFormat::A8B8G8R8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2ABGR };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2ABGR };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2ABGR };
		default:
			return {};
		}
	case PixelFormat::A8G8R8B8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2ARGB };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2ARGB };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2ARGB };
		default:
			return {};
		}
	case PixelFormat::A8G8B8R8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2AGBR };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2AGBR };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2AGBR };
		default:
			return {};
		}
	case PixelFormat::R8G8B8A8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, IDENTITY };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, IDENTITY };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B8G8R8A8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2BGRA };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2BGRA };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G8R8B8A8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2GRBA };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2GRBA };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G8B8R8A8_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA8B8G8R8UnormPack32, RGBA2GBRA };
		case ColorEncoding::INT:
			return { vk::Format::eA8B8G8R8SnormPack32, RGBA2GBRA };
		case ColorEncoding::IEC61966_2_1:
			return { vk::Format::eA8B8G8R8SrgbPack32, RGBA2GBRA };
		default:
			return {};
		}

	case PixelFormat::R8G8B8G8_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB8G8R8G8422Unorm, BGRA2RGBA };
		default:
			return {};
		}
	case PixelFormat::B8G8R8G8_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB8G8R8G8422Unorm, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G8R8G8B8_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG8B8G8R8422Unorm, BGRA2RGBA };
		default:
			return {};
		}
	case PixelFormat::G8B8G8R8_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG8B8G8R8422Unorm, IDENTITY };
		default:
			return {};
		}

	case PixelFormat::G8_B8_R8:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG8B8R83Plane444Unorm, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G8_B8_R8_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG8B8R83Plane422Unorm, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G8_B8_R8_S420:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG8B8R83Plane420Unorm, IDENTITY };
		default:
			return {};
		}

	case PixelFormat::G8_R8B8_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG8B8R82Plane422Unorm, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G8_B8R8_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG8B8R82Plane422Unorm, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G8_R8B8_S420:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG8B8R82Plane420Unorm, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G8_B8R8_S420:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG8B8R82Plane420Unorm, IDENTITY };
		default:
			return {};
		}


	//10 bits per component
	case PixelFormat::Y10X6_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR10X6UnormPack16, R2Y };
		default:
			return {};
		}

	case PixelFormat::Y10X6A10X6_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR10X6G10X6Unorm2Pack16, RG2YA };
		default:
			return {};
		}
	case PixelFormat::A10X6Y10X6_16:
	switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR10X6G10X6Unorm2Pack16, RG2AY };
		default:
			return {};
	}

	case PixelFormat::A10X6R10X6G10X6B10X6_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2ARGB };
		default:
			return {};
		}
	case PixelFormat::A10X6B10X6G10X6R10X6_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2ABGR };
		default:
			return {};
		}
	case PixelFormat::A10X6G10X6R10X6B10X6_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2AGRB };
		default:
			return {};
		}
	case PixelFormat::A10X6G10X6B10X6R10X6_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2AGBR };
		default:
			return {};
		}
	case PixelFormat::R10X6G10X6B10X6A10X6_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B10X6G10X6R10X6A10X6_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G10X6R10X6B10X6A10X6_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G10X6B10X6R10X6A10X6_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, RGBA2GBRA };
		default:
			return {};
		}


	case PixelFormat::X2R10G10B10_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XRGB };
		default:
			return {};
		}
	case PixelFormat::X2B10G10R10_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XBGR };
		default:
			return {};
	}
	case PixelFormat::X2G10R10B10_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XGRB };
		default:
			return {};
		}
	case PixelFormat::X2G10B10R10_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA2R10G10B10UnormPack32, ARGB2XGBR };
		default:
			return {};
	}

	case PixelFormat::A2R10G10B10_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA2R10G10B10UnormPack32, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::A2B10G10R10_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA2R10G10B10UnormPack32, ARGB2ABGR };
		default:
			return {};
	}
	case PixelFormat::A2G10R10B10_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA2R10G10B10UnormPack32, ARGB2AGRB };
		default:
			return {};
		}
	case PixelFormat::A2G10B10R10_32:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eA2R10G10B10UnormPack32, ARGB2AGBR };
		default:
			return {};
	}

	case PixelFormat::R10X6G10X6B10X6G10X6_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16, BGRA2RGBA };
		default:
			return {};
		}
	case PixelFormat::B10X6G10X6R10X6G10X6_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G10X6R10X6G10X6B10X6_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16, BGRA2RGBA };
		default:
			return {};
		}
	case PixelFormat::G10X6B10X6G10X6R10X6_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16, IDENTITY };
		default:
			return {};
		}

	case PixelFormat::G10X6_B10X6_R10X6_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G10X6_B10X6_R10X6_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G10X6_B10X6_R10X6_S420_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16, IDENTITY };
		default:
			return {};
		}

	case PixelFormat::G10X6_R10X6B10X6_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G10X6_B10X6R10X6_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G10X6_R10X6B10X6_S420_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G10X6_B10X6R10X6_S420_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16, IDENTITY };
		default:
			return {};
		}


	//12 bits per component
	case PixelFormat::Y12X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR12X4UnormPack16, R2Y };
		default:
			return {};
		}

	case PixelFormat::Y12X4A12X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR12X4G12X4Unorm2Pack16, RG2YA };
		default:
			return {};
		}
	case PixelFormat::A12X4Y12X4_16:
	switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR12X4G12X4Unorm2Pack16, RG2AY };
		default:
			return {};
	}

	case PixelFormat::A12X4R12X4G12X4B12X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2ARGB };
		default:
			return {};
		}
	case PixelFormat::A12X4B12X4G12X4R12X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2ABGR };
		default:
			return {};
		}
	case PixelFormat::A12X4G12X4R12X4B12X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2AGRB };
		default:
			return {};
		}
	case PixelFormat::A12X4G12X4B12X4R12X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2AGBR };
		default:
			return {};
		}
	case PixelFormat::R12X4G12X4B12X4A12X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B12X4G12X4R12X4A12X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G12X4R12X4B12X4A12X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G12X4B12X4R12X4A12X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, RGBA2GBRA };
		default:
			return {};
		}

	case PixelFormat::R12X4G12X4B12X4G12X4_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16, BGRA2RGBA };
		default:
			return {};
		}
	case PixelFormat::B12X4G12X4R12X4G12X4_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G12X4R12X4G12X4B12X4_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16, BGRA2RGBA };
		default:
			return {};
		}
	case PixelFormat::G12X4B12X4G12X4R12X4_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16, IDENTITY };
		default:
			return {};
		}

	case PixelFormat::G12X4_B12X4_R12X4_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G12X4_B12X4_R12X4_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G12X4_B12X4_R12X4_S420_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16, IDENTITY };
		default:
			return {};
		}

	case PixelFormat::G12X4_R12X4B12X4_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G12X4_B12X4R12X4_S422_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G12X4_R12X4B12X4_S420_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G12X4_B12X4R12X4_S420_16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16, IDENTITY };
		default:
			return {};
		}


	//16 bits per component
	case PixelFormat::Y16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16Unorm, R2Y };
		case ColorEncoding::INT:
			return { vk::Format::eR16Snorm, R2Y };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16Sfloat, R2Y };
		default:
			return {};
		}

	case PixelFormat::Y16A16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16Unorm, RG2YA };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16Snorm, RG2YA };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16Sfloat, RG2YA };
		default:
			return {};
		}
	case PixelFormat::A16Y16:
	switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16Unorm, RG2AY };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16Snorm, RG2AY };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16Sfloat, RG2AY };
		default:
			return {};
	}
		
	case PixelFormat::R16G16B16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16Unorm, IDENTITY };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16Snorm, IDENTITY };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16Sfloat, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B16G16R16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16Unorm, RGBA2BGRA };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16Snorm, RGBA2BGRA };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16Sfloat, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G16R16B16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16Unorm, RGBA2GRBA };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16Snorm, RGBA2GRBA };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16Sfloat, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G16B16R16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16Unorm, RGBA2GBRA };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16Snorm, RGBA2GBRA };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16Sfloat, RGBA2GBRA };
		default:
			return {};
		}

	case PixelFormat::A16R16G16B16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16A16Unorm, RGBA2ARGB };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16A16Snorm, RGBA2ARGB };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16A16Sfloat, RGBA2ARGB };
		default:
			return {};
		}
	case PixelFormat::A16B16G16R16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16A16Unorm, RGBA2ABGR };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16A16Snorm, RGBA2ABGR };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16A16Sfloat, RGBA2ABGR };
		default:
			return {};
		}
	case PixelFormat::A16G16R16B16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16A16Unorm, RGBA2AGRB };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16A16Snorm, RGBA2AGRB };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16A16Sfloat, RGBA2AGRB };
		default:
			return {};
		}
	case PixelFormat::A16G16B16R16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16A16Unorm, RGBA2AGBR };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16A16Snorm, RGBA2AGBR };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16A16Sfloat, RGBA2AGBR };
		default:
			return {};
		}
	case PixelFormat::R16G16B16A16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16A16Unorm, IDENTITY };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16A16Snorm, IDENTITY };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16A16Sfloat, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B16G16R16A16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16A16Unorm, RGBA2BGRA };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16A16Snorm, RGBA2BGRA };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16A16Sfloat, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G16R16B16A16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16A16Unorm, RGBA2GRBA };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16A16Snorm, RGBA2GRBA };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16A16Sfloat, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G16B16R16A16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eR16G16B16A16Unorm, RGBA2GBRA };
		case ColorEncoding::INT:
			return { vk::Format::eR16G16B16A16Snorm, RGBA2GBRA };
		case ColorEncoding::IEEE754:
			return { vk::Format::eR16G16B16A16Sfloat, RGBA2GBRA };
		default:
			return {};
		}

	case PixelFormat::R16G16B16G16_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB16G16R16G16422Unorm, BGRA2RGBA };
		default:
			return {};
		}
	case PixelFormat::B16G16R16G16_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eB16G16R16G16422Unorm, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G16R16G16B16_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG16B16G16R16422Unorm, BGRA2RGBA };
		default:
			return {};
		}
	case PixelFormat::G16B16G16R16_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG16B16G16R16422Unorm, IDENTITY };
		default:
			return {};
		}

	case PixelFormat::G16_B16_R16:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG16B16R163Plane444Unorm, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G16_B16_R16_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG16B16R163Plane422Unorm, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G16_B16_R16_S420:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG16B16R163Plane420Unorm, IDENTITY };
		default:
			return {};
		}

	case PixelFormat::G16_R16B16_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG16B16R162Plane422Unorm, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G16_B16R16_S422:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG16B16R162Plane422Unorm, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::G16_R16B16_S420:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG16B16R162Plane420Unorm, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G16_B16R16_S420:
		switch(enc){
		case ColorEncoding::UINT:
			return { vk::Format::eG16B16R162Plane420Unorm, IDENTITY };
		default:
			return {};
		}
	

	//32 bits per component
	case PixelFormat::Y32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32Sfloat, R2Y };
		default:
			return {};
		}

	case PixelFormat::Y32A32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32Sfloat, RG2YA };
		default:
			return {};
		}
	case PixelFormat::A32Y32:
	switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32Sfloat, RG2AY };
		default:
			return {};
	}
		
	case PixelFormat::R32G32B32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32Sfloat, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B32G32R32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32Sfloat, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G32R32B32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32Sfloat, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G32B32R32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32Sfloat, RGBA2GBRA };
		default:
			return {};
		}

	case PixelFormat::A32R32G32B32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32A32Sfloat, RGBA2ARGB };
		default:
			return {};
		}
	case PixelFormat::A32B32G32R32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32A32Sfloat, RGBA2ABGR };
		default:
			return {};
		}
	case PixelFormat::A32G32R32B32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32A32Sfloat, RGBA2AGRB };
		default:
			return {};
		}
	case PixelFormat::A32G32B32R32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32A32Sfloat, RGBA2AGBR };
		default:
			return {};
		}
	case PixelFormat::R32G32B32A32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32A32Sfloat, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B32G32R32A32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32A32Sfloat, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G32R32B32A32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32A32Sfloat, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G32B32R32A32:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR32G32B32A32Sfloat, RGBA2GBRA };
		default:
			return {};
		}


	//64 bits per component
	case PixelFormat::Y64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64Sfloat, R2Y };
		default:
			return {};
		}

	case PixelFormat::Y64A64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64Sfloat, RG2YA };
		default:
			return {};
		}
	case PixelFormat::A64Y64:
	switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64Sfloat, RG2AY };
		default:
			return {};
	}
		
	case PixelFormat::R64G64B64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64Sfloat, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B64G64R64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64Sfloat, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G64R64B64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64Sfloat, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G64B64R64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64Sfloat, RGBA2GBRA };
		default:
			return {};
		}

	case PixelFormat::A64R64G64B64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64A64Sfloat, RGBA2ARGB };
		default:
			return {};
		}
	case PixelFormat::A64B64G64R64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64A64Sfloat, RGBA2ABGR };
		default:
			return {};
		}
	case PixelFormat::A64G64R64B64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64A64Sfloat, RGBA2AGRB };
		default:
			return {};
		}
	case PixelFormat::A64G64B64R64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64A64Sfloat, RGBA2AGBR };
		default:
			return {};
		}
	case PixelFormat::R64G64B64A64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64A64Sfloat, IDENTITY };
		default:
			return {};
		}
	case PixelFormat::B64G64R64A64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64A64Sfloat, RGBA2BGRA };
		default:
			return {};
		}
	case PixelFormat::G64R64B64A64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64A64Sfloat, RGBA2GRBA };
		default:
			return {};
		}
	case PixelFormat::G64B64R64A64:
		switch(enc){
		case ColorEncoding::IEEE754:
			return { vk::Format::eR64G64B64A64Sfloat, RGBA2GBRA };
		default:
			return {};
		}

	default: //NONE
			return {};
	}
}

constexpr std::tuple<PixelFormat, ColorEncoding> fromVulkan(vk::Format fmt){

}

}
