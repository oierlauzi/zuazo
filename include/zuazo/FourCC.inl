#include "FourCC.h"

#include "Utils/CPU.h"

namespace Zuazo {

constexpr std::tuple<ColorFormat, ColorSubsampling, ColorModel> fromFourCC(	FourCC fcc, 
																			ColorModel ycbcrModel ) noexcept
{
	//Based on:
	//https://chromium.googlesource.com/libyuv/libyuv/+/HEAD/docs/formats.md
	//https://docs.microsoft.com/en-us/windows/win32/medfound/10-bit-and-16-bit-yuv-video-formats
	//https://www.fourcc.org/yuv.php
	//TODO consider color range (described in first doc)

	switch(fcc) {
	case FourCC::RGBX:
	case FourCC::RGBA:	return { ColorFormat::R8G8B8A8,											ColorSubsampling::rb444, 	ColorModel::rgb };
	case FourCC::XRGB:
	case FourCC::ARGB:	return { ColorFormat::A8R8G8B8,											ColorSubsampling::rb444, 	ColorModel::rgb };
	case FourCC::BGRX:
	case FourCC::BGRA:	return { ColorFormat::B8G8R8A8,											ColorSubsampling::rb444, 	ColorModel::rgb };
	case FourCC::XBGR:
	case FourCC::ABGR:	return { ColorFormat::A8B8G8R8,											ColorSubsampling::rb444, 	ColorModel::rgb };

	case FourCC::Y800:																									
	case FourCC::GREY:	return { ColorFormat::Y8, 												ColorSubsampling::none,		ColorModel::rgb };		

	case FourCC::UYVY:	return { ColorFormat::B8G8R8G8,											ColorSubsampling::rb422,	ycbcrModel };	
	case FourCC::HDYC:	return { ColorFormat::B8G8R8G8,											ColorSubsampling::rb422,	ColorModel::bt709 };	
	case FourCC::YVYU:	return { ColorFormat::G8R8G8B8,											ColorSubsampling::rb422,	ycbcrModel }; 	
	case FourCC::YUYV:	
	case FourCC::YUY2:	return { ColorFormat::G8B8G8R8,											ColorSubsampling::rb422,	ycbcrModel };

	case FourCC::Y216:	return { ColorFormat::G16B16G16R16,										ColorSubsampling::rb422, 	ColorModel::rgb };
	case FourCC::Y210:	return { ColorFormat::G10X6B10X6G10X6R10X6_16,							ColorSubsampling::rb422, 	ColorModel::rgb };
	case FourCC::Y416:	return { ColorFormat::A16R16G16B16,										ColorSubsampling::rb444, 	ColorModel::rgb };
	case FourCC::Y410:	return { ColorFormat::A2R10G10B10_32,									ColorSubsampling::rb444, 	ColorModel::rgb };

	//case FourCC::AYUV:	return { ColorFormat::A8G8B8R8,										ColorSubsampling::rb444, 	ycbcrModel };
	//case FourCC::UYVA:	return { ColorFormat::B8G8R8A8,										ColorSubsampling::rb444, 	ycbcrModel };

	case FourCC::NV12:	return { ColorFormat::G8_B8R8,											ColorSubsampling::rb420,	ycbcrModel };
	case FourCC::NV21:	return { ColorFormat::G8_R8B8,											ColorSubsampling::rb420,	ycbcrModel };
	case FourCC::NV16:	return { ColorFormat::G8_B8R8,											ColorSubsampling::rb422,	ycbcrModel };
	case FourCC::NV61:	return { ColorFormat::G8_R8B8,											ColorSubsampling::rb422,	ycbcrModel };
	case FourCC::NV24:	return { ColorFormat::G8_B8R8,											ColorSubsampling::rb444,	ycbcrModel };
	case FourCC::NV42:	return { ColorFormat::G8_R8B8,											ColorSubsampling::rb444,	ycbcrModel };

	case FourCC::P016:	return { ColorFormat::G16_B16R16,										ColorSubsampling::rb420,	ycbcrModel };
	case FourCC::P010:	return { ColorFormat::G10X6_B10X6R10X6_16,								ColorSubsampling::rb420,	ycbcrModel };
	case FourCC::P216:	return { ColorFormat::G16_B16R16,										ColorSubsampling::rb422,	ycbcrModel };
	case FourCC::P210:	return { ColorFormat::G10X6_B10X6R10X6_16,								ColorSubsampling::rb422,	ycbcrModel };

	case FourCC::PA16:	return { ColorFormat::G16_B16R16_A16,									ColorSubsampling::rb422,	ycbcrModel };

	case FourCC::I420:	return { ColorFormat::G8_B8_R8,											ColorSubsampling::rb420,	ycbcrModel }; 
	case FourCC::I422:	return { ColorFormat::G8_B8_R8,											ColorSubsampling::rb422,	ycbcrModel }; 
	case FourCC::I444:	return { ColorFormat::G8_B8_R8,											ColorSubsampling::rb444,	ycbcrModel }; 

	//case FourCC::YV12:	/*TODO :NOT IMPLEMENTED*/
	//case FourCC::YV16:	/*TODO :NOT IMPLEMENTED*/

	default:			return { ColorFormat::none,												ColorSubsampling::none,		ColorModel::none }; 
	}
}


inline std::string_view toString(const FourCC& fcc) noexcept {
	return std::string_view(
		reinterpret_cast<const char*>(fcc),
		sizeof(fcc) / sizeof(char)
	);
}

inline std::ostream& operator<<(std::ostream& os, FourCC fcc) {
	return os << toString(fcc);
}

}