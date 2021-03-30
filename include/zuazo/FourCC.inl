#include "FourCC.h"

#include "Utils/CPU.h"

namespace Zuazo {

constexpr std::tuple<ColorFormat, ColorSubsampling, ColorModel> fromFourCC(FourCC fcc) noexcept {
	switch(fcc) {
	case FourCC::RGBX:	return { ColorFormat::R8G8B8X8_32,										ColorSubsampling::RB_444, 	ColorModel::RGB };
	case FourCC::RGBA:	return { ColorFormat::R8G8B8A8_32,										ColorSubsampling::RB_444, 	ColorModel::RGB };
	case FourCC::BGRX:	return { ColorFormat::B8G8R8X8_32,										ColorSubsampling::RB_444, 	ColorModel::RGB };
	case FourCC::BGRA:	return { ColorFormat::B8G8R8A8_32,										ColorSubsampling::RB_444, 	ColorModel::RGB };

	case FourCC::Y800:																									
	case FourCC::GREY:	return { ColorFormat::Y8, 												ColorSubsampling::NONE,		ColorModel::RGB };		

	case FourCC::UYVY:	return { Utils::bele(ColorFormat::B8G8R8G8, ColorFormat::G8R8G8B8),		ColorSubsampling::RB_422,	ColorModel::BT601 };	
	case FourCC::HDYC:	return { Utils::bele(ColorFormat::B8G8R8G8, ColorFormat::G8R8G8B8),		ColorSubsampling::RB_422,	ColorModel::BT709 };	
	case FourCC::YVYU:	return { Utils::bele(ColorFormat::G8R8G8B8, ColorFormat::B8G8R8G8),		ColorSubsampling::RB_422,	ColorModel::BT601 }; 	
	case FourCC::YUYV:	
	case FourCC::YUY2:	return { Utils::bele(ColorFormat::G8B8G8R8, ColorFormat::R8G8B8G8),		ColorSubsampling::RB_422,	ColorModel::BT601 };

	case FourCC::Y216:	return { ColorFormat::G16B16G16R16,										ColorSubsampling::RB_422, 	ColorModel::RGB };
	case FourCC::Y210:	return { ColorFormat::G10X6B10X6G10X6R10X6_16,							ColorSubsampling::RB_422, 	ColorModel::RGB };
	case FourCC::Y416:	return { ColorFormat::A16R16G16B16,										ColorSubsampling::RB_444, 	ColorModel::RGB };
	case FourCC::Y410:	return { ColorFormat::A2R10G10B10_32,									ColorSubsampling::RB_444, 	ColorModel::RGB };

	case FourCC::AYUV:	return { ColorFormat::A8G8B8R8,											ColorSubsampling::RB_444, 	ColorModel::BT601 };
	case FourCC::UYVA:	return { ColorFormat::B8G8R8A8,											ColorSubsampling::RB_444, 	ColorModel::BT601 };

	case FourCC::NV12:	return { ColorFormat::G8_B8R8,											ColorSubsampling::RB_420,	ColorModel::BT601 };
	case FourCC::NV21:	return { ColorFormat::G8_R8B8,											ColorSubsampling::RB_420,	ColorModel::BT601 };

	case FourCC::P016:	return { ColorFormat::G16_B16R16,										ColorSubsampling::RB_420,	ColorModel::BT601 };
	case FourCC::P010:	return { ColorFormat::G10X6_B10X6R10X6_16,								ColorSubsampling::RB_420,	ColorModel::BT601 };
	case FourCC::P216:	return { ColorFormat::G16_B16R16,										ColorSubsampling::RB_422,	ColorModel::BT601 };
	case FourCC::P210:	return { ColorFormat::G10X6_B10X6R10X6_16,								ColorSubsampling::RB_422,	ColorModel::BT601 };

	//case FourCC::PA16:	/*TODO :NOT IMPLEMENTED*/

	//case FourCC::YV12:	/*TODO :NOT IMPLEMENTED*/
	case FourCC::I420:	return { ColorFormat::G8_B8_R8,											ColorSubsampling::RB_420,	ColorModel::BT601 }; 
	//case FourCC::YV16:	/*TODO :NOT IMPLEMENTED*/

	default:			return {};
	}
}

}